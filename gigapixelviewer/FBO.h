#ifndef FBO_H
#define FBO_H

#include "common.h"

namespace GiGaPixelViewer
{
	class FBO
	{
	public:
		FBO();
		~FBO();
		void CreateFBO(const int w, const int h);
		void Bind();
		void Unbind();

		unsigned int texture() const; 
		void setTexture(unsigned int val);
		int width() const; 
		void setWidth(int val);
		int height() const; 
		void setHeight(int val); 


	private:
		GLuint myFbo;
		GLuint myTexture;
		int myWidth;
		int myHeight;
	};

}


#endif