#version 410


out vec4 frag_colour;
uniform int hitWall;

void main () {
    if(hitWall == 1)
	{
	 frag_colour = vec4 (0.0, 0.0, 1.0, 1.0);
	}
	else
	{
	  frag_colour = vec4 (1.0, 1.0, 1.0, 1.0);
	}
	
}