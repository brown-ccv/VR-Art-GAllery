#include "FBO.h"
#include "common.h"

namespace GiGaPixelViewer
{
	FBO::FBO()
	{
	}


	FBO::~FBO()
	{
	}

	void FBO::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, myFbo);
	}

	void FBO::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	unsigned int FBO::texture() const
	{
		 return myTexture; 
	}

	void FBO::setTexture(unsigned int val)
	{
		myTexture = val;
	}

	int FBO::width() const
	{
		return myWidth;
	}

	void FBO::setWidth(int val)
	{
		myWidth = val;
	}

	int FBO::height() const
	{
		return myHeight;
	}

	void FBO::setHeight(int val)
	{
		myHeight = val;
	}

	void FBO::CreateFBO(const int w, const int h)
	{
		myWidth = w;
		myHeight = h;

		glGenFramebuffers(1, &myFbo);
		glBindFramebuffer(GL_FRAMEBUFFER, myFbo);

		// Create a render buffer, and attach it to FBO's depth attachment
		unsigned int depthBuffer;
		glGenRenderbuffers(1, &depthBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
			width(), height());
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
			GL_RENDERBUFFER, depthBuffer);

		// Create texture and attach FBO's color 0 attachment
		glGenTextures(1, &myTexture);
		glBindTexture(GL_TEXTURE_2D, myTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, myWidth, myHeight,
			0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glFramebufferTexture2D(GL_FRAMEBUFFER,
			GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D, myTexture, 0);

		// Check for completeness/correctness
		int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE)
		{
			assert(false && "FBO error");
		}

		// Unbind the fbo.
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}



}