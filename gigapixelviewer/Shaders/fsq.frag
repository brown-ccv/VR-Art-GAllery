#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D screenTexture;

out vec4 color;
void main()
{ 
    
   
	
	color = texture(screenTexture, TexCoords);
	
	/*if(color.a != 0)
	{
	  color = vec4(0.5,0.5,0.5,1);
	}*/
}