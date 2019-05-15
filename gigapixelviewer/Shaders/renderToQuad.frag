#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform vec3 tileCoordinates;

uniform int secondHalf;
uniform int highLight;
uniform int centerTile;

void main()
{ 
    
   float maxX = 1.0 - 0.015;
   float minX = 0.015;
   float maxY = maxX / 1;
   float minY = minX / 1;


    if (TexCoords.x < maxX && TexCoords.x > minX &&
       TexCoords.y < maxY && TexCoords.y > minY) 
   {
      vec2 flipY; 
	  flipY.x = TexCoords.x;
	  flipY.y = 1.0 - TexCoords.y;

	  FragColor = texture(screenTexture, flipY);   
	  if(secondHalf == 1)
	  {
	      FragColor *=  vec4(0.65,0.1,0.1,1.0);
	  }

   } 
   else 
   {
     FragColor = vec4(0,0,0,1);
   }

   	if(highLight  == 1 )
	{	  
	   FragColor *=  vec4(0.75,1.f,0.75,0.8);
	}

	if(centerTile == 1)
	{
	  FragColor *=  vec4(0.1,0.1f,0.75,0.95);
	}

	//FragColor = vec4(0.5,0.5,0.5,1);
}