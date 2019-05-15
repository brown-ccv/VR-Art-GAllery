#version 410

out vec4 frag_colour;
uniform int isPoint;

void main () {

     if(isPoint == 0)
	 {
	  frag_colour = vec4 (1.0, 0.5, 0.75, 1.0); 
	 }
	 else
	 {
	  frag_colour = vec4 (1.0, 0.9, 1.0, 1.0); 
	 }
	
}