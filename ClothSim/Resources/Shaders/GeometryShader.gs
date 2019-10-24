#version 430 core 

layout (points) in;
layout (triangle_strip, max_vertices = 23) out;

out vec3 outColor;

in VS_GS_VERTEX{ 

 in vec4 position;
 in vec3 color; 
 in mat4 mvp; 
 
}gs_in[];

void main() {   
 
	outColor = gs_in[0].color;
	
	//Top Triangle
	gl_Position = gs_in[0].position + gs_in[0].mvp * vec4(-0.15f, 0.25f, 0.0f, 0.0f);
	EmitVertex();
	
	gl_Position = gs_in[0].position + gs_in[0].mvp * vec4(0.15f, 0.25f, 0.0f, 0.0f);
	EmitVertex();
	
	gl_Position = gs_in[0].position + gs_in[0].mvp * vec4(0.0f, 0.75f, 0.0f, 0.0f);
	EmitVertex();
	EndPrimitive();
	
	//Top Right Triangle
	gl_Position = gs_in[0].position + gs_in[0].mvp * vec4(0.15f, 0.25, 0.0f, 0.0f);
	EmitVertex();
	
	gl_Position = gs_in[0].position + gs_in[0].mvp * vec4(0.25f, -0.15f, 0.0f, 0.0f);
	EmitVertex();
	
	gl_Position = gs_in[0].position + gs_in[0].mvp * vec4(0.65f, 0.25f, 0.0f, 0.0f);
	EmitVertex();
	EndPrimitive();
	//Bottom Right Triangle
	gl_Position = gs_in[0].position + gs_in[0].mvp * vec4(0.25f, -0.15f, 0.0f, 0.0f);
	EmitVertex();
	
	gl_Position = gs_in[0].position + gs_in[0].mvp * vec4(0.0f, -0.35f, 0.0f, 0.0f);
	EmitVertex();
	
	gl_Position = gs_in[0].position + gs_in[0].mvp * vec4(0.45f, -0.75f, 0.0f, 0.0f);
	EmitVertex();
	EndPrimitive();
	
	//Bottom Left Triangle
	gl_Position = gs_in[0].position + gs_in[0].mvp * vec4(0.0f, -0.35f, 0.0f, 0.0f);
	EmitVertex();
	
	gl_Position = gs_in[0].position + gs_in[0].mvp * vec4(-0.25f, -0.15f, 0.0f, 0.0f);
	EmitVertex();
	
	gl_Position = gs_in[0].position + gs_in[0].mvp * vec4(-0.45f, -0.75f, 0.0f, 0.0f);
	EmitVertex();
	EndPrimitive();
	
	//Top Left Triangle
	gl_Position = gs_in[0].position + gs_in[0].mvp * vec4(-0.25f, -0.15f, 0.0f, 0.0f);
	EmitVertex();
	
	gl_Position = gs_in[0].position + gs_in[0].mvp * vec4(-0.15f, 0.25f, 0.0f, 0.0f);
	EmitVertex();
	
	gl_Position = gs_in[0].position + gs_in[0].mvp * vec4(-0.65f, 0.25f, 0.0f, 0.0f);
	EmitVertex();
	EndPrimitive();
	
	//Bottom Inside Triangle
	gl_Position = gs_in[0].position + gs_in[0].mvp * vec4(0.0f, -0.35f, 0.0f, 0.0f);
	EmitVertex();
	
	gl_Position = gs_in[0].position + gs_in[0].mvp * vec4(0.25f, -0.15f, 0.0f, 0.0f);
	EmitVertex();
	
	gl_Position = gs_in[0].position + gs_in[0].mvp * vec4(-0.25f, -0.15f, 0.0f, 0.0f);
	EmitVertex();
	EndPrimitive();
	
	//Final Quad
	gl_Position = gs_in[0].position + gs_in[0].mvp * vec4(-0.25f, -0.15f, 0.0f, 0.0f);
	EmitVertex();
	
	gl_Position = gs_in[0].position + gs_in[0].mvp * vec4(0.25f, -0.15f, 0.0f, 0.0f);
	EmitVertex();
	
	gl_Position = gs_in[0].position + gs_in[0].mvp * vec4(-0.15f, 0.25f, 0.0f, 0.0f);
	EmitVertex();
	
	gl_Position = gs_in[0].position + gs_in[0].mvp * vec4(0.15f, 0.25f, 0.0f, 0.0f);
	EmitVertex();
	EndPrimitive();
}