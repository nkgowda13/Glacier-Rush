#include "CatmullRom.h"
#define _USE_MATH_DEFINES
#include <math.h>



CCatmullRom::CCatmullRom()
{
	m_vertexCount = 0;
}

CCatmullRom::~CCatmullRom()
{}

// Perform Catmull Rom spline interpolation between four points, interpolating the space between p1 and p2
glm::vec3 CCatmullRom::Interpolate(glm::vec3& p0, glm::vec3& p1, glm::vec3& p2, glm::vec3& p3, float t)
{
	float t2 = t * t;
	float t3 = t2 * t;

	glm::vec3 a = p1;
	glm::vec3 b = 0.5f * (-p0 + p2);
	glm::vec3 c = 0.5f * (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3);
	glm::vec3 d = 0.5f * (-p0 + 3.0f * p1 - 3.0f * p2 + p3);

	return a + b * t + c * t2 + d * t3;

}

// Determine lengths along the control points, which is the set of control points forming the closed curve
void CCatmullRom::ComputeLengthsAlongControlPoints()
{
	int M = (int)m_controlPoints.size();

	float fAccumulatedLength = 0.0f;
	m_distances.push_back(fAccumulatedLength);
	for (int i = 1; i < M; i++) {
		fAccumulatedLength += glm::distance(m_controlPoints[i - 1], m_controlPoints[i]);
		m_distances.push_back(fAccumulatedLength);
	}

	// Get the distance from the last point to the first
	fAccumulatedLength += glm::distance(m_controlPoints[M - 1], m_controlPoints[0]);
	m_distances.push_back(fAccumulatedLength);
}

// Return the point (and upvector, if control upvectors provided) based on a distance d along the control polygon
bool CCatmullRom::Sample(float d, glm::vec3& p, glm::vec3& up)
{
	if (d < 0)
		return false;

	int M = (int)m_controlPoints.size();
	if (M == 0)
		return false;


	float fTotalLength = m_distances[m_distances.size() - 1];

	// The the current length along the control polygon; handle the case where we've looped around the track
	float fLength = d - (int)(d / fTotalLength) * fTotalLength;

	// Find the current segment
	int j = -1;
	for (int i = 0; i < (int)m_distances.size() - 1; i++) {
		if (fLength >= m_distances[i] && fLength < m_distances[i + 1]) {
			j = i; // found it!
			break;
		}
	}

	if (j == -1)
		return false;

	// Interpolate on current segment -- get t
	float fSegmentLength = m_distances[j + 1] - m_distances[j];
	float t = (fLength - m_distances[j]) / fSegmentLength;

	// Get the indices of the four points along the control polygon for the current segment
	int iPrev = ((j - 1) + M) % M;
	int iCur = j;
	int iNext = (j + 1) % M;
	int iNextNext = (j + 2) % M;

	// Interpolate to get the point (and upvector)
	p = Interpolate(m_controlPoints[iPrev], m_controlPoints[iCur], m_controlPoints[iNext], m_controlPoints[iNextNext], t);
	if (m_controlUpVectors.size() == m_controlPoints.size())
		up = glm::normalize(Interpolate(m_controlUpVectors[iPrev], m_controlUpVectors[iCur], m_controlUpVectors[iNext], m_controlUpVectors[iNextNext], t));

	return true;
}

// Sample a set of control points using an open Catmull-Rom spline, to produce a set of iNumSamples that are (roughly) equally spaced
void CCatmullRom::UniformlySampleControlPoints(int numSamples)
{
	glm::vec3 p, up;

	// Compute the lengths of each segment along the control polygon, and the total length
	ComputeLengthsAlongControlPoints();
	float fTotalLength = m_distances[m_distances.size() - 1];

	// The spacing will be based on the control polygon
	float fSpacing = fTotalLength / numSamples;

	// Call PointAt to sample the spline, to generate the points
	for (int i = 0; i < numSamples; i++) {
		Sample(i * fSpacing, p, up);
		m_centrelinePoints.push_back(p);
		if (m_controlUpVectors.size() > 0)
			m_centrelineUpVectors.push_back(up);

	}


	// Repeat once more for truly equidistant points
	m_controlPoints = m_centrelinePoints;
	m_controlUpVectors = m_centrelineUpVectors;
	m_centrelinePoints.clear();
	m_centrelineUpVectors.clear();
	m_distances.clear();
	ComputeLengthsAlongControlPoints();
	fTotalLength = m_distances[m_distances.size() - 1];
	fSpacing = fTotalLength / numSamples;
	for (int i = 0; i < numSamples; i++) {
		Sample(i * fSpacing, p, up);
		m_centrelinePoints.push_back(p);
		if (m_controlUpVectors.size() > 0)
			m_centrelineUpVectors.push_back(up);
	}


}

// Create the centre line for the path using the control points provided
void CCatmullRom::CreateCentreline(std::vector<glm::vec3> controlPoints)
{
	// Set control points (m_controlPoints) here
	m_controlPoints = controlPoints;

	// Call UniformlySampleControlPoints with the number of samples required
	UniformlySampleControlPoints(500);

	// Create a VAO called m_vaoCentreline and a VBO to get the points onto the graphics card
	glGenVertexArrays(1, &m_vaoCentreline);
	glBindVertexArray(m_vaoCentreline);
	CVertexBufferObject vbo;
	vbo.Create();
	vbo.Bind();

	// set the texture coordinates and normal for the points in the VBO
	glm::vec2 texCoord(0.0f, 0.0f);
	glm::vec3 normal(0.0f, 1.0f, 0.0f);

	// add the data to every point in the centreline  
	for (unsigned int i = 0; i < m_centrelinePoints.size(); i++)
	{
		glm::vec3 v = m_centrelinePoints[i];
		vbo.AddData(&v, sizeof(glm::vec3));
		vbo.AddData(&texCoord, sizeof(glm::vec2));
		vbo.AddData(&normal, sizeof(glm::vec3));
	}

	// Upload the VBO to the GPU
	vbo.UploadDataToGPU(GL_STATIC_DRAW);
	// Set the vertex attribute locations
	GLsizei stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);
	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));
}

// Compute the offset curves, one left, and one right.  Store the points in m_leftOffsetPoints and m_rightOffsetPoints respectively
void CCatmullRom::CreateOffsetCurves(float width)
{
	// use TNB frame to calculate positions of points in left and right offset curves
	for (int i = 0; i < m_centrelinePoints.size(); i++)
	{
		glm::vec3 p = m_centrelinePoints[i];
		glm::vec3 y = glm::vec3(0, 1, 0);
		glm::vec3 pNext = m_centrelinePoints[(i + 1) % sampleNum];
		glm::vec3 T = normalize(pNext - p);
		glm::vec3 N = glm::vec3(glm::cross(T, y));

		m_leftOffsetPoints.push_back(p - ((width / 2) * N));
		m_rightOffsetPoints.push_back(p + ((width / 2) * N));
	}

	// set the txture coordinates and normal for the points in the VBO
	glm::vec2 texCoord(0.0f, 0.0f);
	glm::vec3 normal(0.0f, 1.0f, 0.0f);

	// Set the stride length
	GLsizei stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// LEFT OFFSET CURVE VBO
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	glGenVertexArrays(1, &m_vaoLeftOffsetCurve);
	glBindVertexArray(m_vaoLeftOffsetCurve);

	CVertexBufferObject leftVBO;
	leftVBO.Create();
	leftVBO.Bind();

	// use a for loop to add points to vbo 
	for (unsigned int i = 0; i < m_leftOffsetPoints.size(); i++)
	{
		glm::vec3 l = m_leftOffsetPoints[i];
		leftVBO.AddData(&l, sizeof(glm::vec3));
		leftVBO.AddData(&texCoord, sizeof(glm::vec2));
		leftVBO.AddData(&normal, sizeof(glm::vec3));
	}

	// Upload the VBO to the GPU
	leftVBO.UploadDataToGPU(GL_STATIC_DRAW);
	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));


	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// RIGHT OFFSET CURVE VBO
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	glGenVertexArrays(1, &m_vaoRightOffsetCurve);
	glBindVertexArray(m_vaoRightOffsetCurve);

	CVertexBufferObject rightVBO;
	rightVBO.Create();
	rightVBO.Bind();

	// use a for loop to add points to vbo 
	for (unsigned int i = 0; i < m_rightOffsetPoints.size(); i++)
	{
		glm::vec3 r = m_rightOffsetPoints[i];
		rightVBO.AddData(&r, sizeof(glm::vec3));
		rightVBO.AddData(&texCoord, sizeof(glm::vec2));
		rightVBO.AddData(&normal, sizeof(glm::vec3));
	}

	// Upload the VBO to the GPU
	rightVBO.UploadDataToGPU(GL_STATIC_DRAW);
	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));
}

// Generate the complete track and upload the data to GPU
void CCatmullRom::CreateTrack(string sDirectory, string sFilename)
{
	// Load the texture
	m_texture.Load(sDirectory + sFilename, true);

	m_directory = sDirectory;
	m_filename = sFilename;

	// Set parameters for texturing using sampler object
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Generate a VAO called m_vaoTrack and a VBO to get the offset curve points and indices on the graphics card
	glGenVertexArrays(1, &m_vaoTrack);
	glBindVertexArray(m_vaoTrack);

	CVertexBufferObject vbo;
	vbo.Create();
	vbo.Bind();

	// set the txture coordinates and normal for the points in the VBO
	vector<glm::vec2> textures = {
		glm::vec2(0.0f, 0.0f),
		glm::vec2(0.0f, 2.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(1.0f, 2.0f)
	};/*
	glm::vec2 texCoord(0.0f, 0.0f);*/
	glm::vec3 normal(0.0f, 1.0f, 0.0f);

	glm::vec2 texCoord;
	glm::vec2 texCoord1;

	// every loop adds a point from the left offset curve, then the right curve, then loops
	// loop also increments m_vertexCount to keep count of total vertex numbers
	for (int i = 0; i < m_centrelinePoints.size() + 1; i++)
	{
		if (i % 2 == 0)
		{
			texCoord = textures[0];
			texCoord1 = textures[1];
		}
		else
		{
			texCoord = textures[2];
			texCoord1 = textures[3];
		}
		glm::vec3 p = m_leftOffsetPoints[i % m_leftOffsetPoints.size()];
		vbo.AddData(&p, sizeof(glm::vec3));
		vbo.AddData(&texCoord, sizeof(glm::vec2));
		vbo.AddData(&normal, sizeof(glm::vec3));

		glm::vec3 q = m_rightOffsetPoints[i % m_rightOffsetPoints.size()];
		vbo.AddData(&q, sizeof(glm::vec3));
		vbo.AddData(&texCoord1, sizeof(glm::vec2));
		vbo.AddData(&normal, sizeof(glm::vec3));

		m_vertexCount = m_vertexCount + 2;
	}

	// Set the vertex attribute locations
	GLsizei stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);
	// Upload the VBO to the GPU
	vbo.UploadDataToGPU(GL_STATIC_DRAW);
	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));

}


void CCatmullRom::RenderCentreline()
{
	// Bind the VAO m_vaoCentreline and render it
	glBindVertexArray(m_vaoCentreline);

	//change the line size and point size so theyre more visible when rendered
	glLineWidth(2);
	glPointSize(5);

	//render the path as both points and as a line loop
	glDrawArrays(GL_POINTS, 0, m_centrelinePoints.size());
	glDrawArrays(GL_LINE_LOOP, 0, m_centrelinePoints.size());
}

void CCatmullRom::RenderOffsetCurves()
{
	// Bind the VAO m_vaoLeftOffsetCurve and render it
	glBindVertexArray(m_vaoLeftOffsetCurve);
	//change the line size and point size so theyre more visible when rendered
	glLineWidth(10);
	glPointSize(3);

	//render the path as both points and as a line loop
	glDrawArrays(GL_POINTS, 0, m_leftOffsetPoints.size());


	// Bind the VAO m_vaoRightOffsetCurve and render it
	glBindVertexArray(m_vaoRightOffsetCurve);
	//change the line size and point size so theyre more visible when rendered
	glLineWidth(10);
	glPointSize(5);

	//render the path as both points and as a line loop
	glDrawArrays(GL_POINTS, 0, m_rightOffsetPoints.size());
}


void CCatmullRom::RenderTrack()
{
	// Bind the VAO m_vaoTrack and texture and then render it
	glBindVertexArray(m_vaoTrack);
	m_texture.Bind();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, m_vertexCount);
}

int CCatmullRom::CurrentLap(float d)
{

	return (int)(d / m_distances.back());

}

glm::vec3 CCatmullRom::_dummy_vector(0.0f, 0.0f, 0.0f);