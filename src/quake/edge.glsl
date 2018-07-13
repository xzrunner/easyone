const char* edge_vs = R"(

attribute vec4 position;

uniform mat4 u_projection;
uniform mat4 u_modelview;

void main()
{
	gl_Position = u_projection * u_modelview * position;
}

)";

const char* edge_fs = R"(

uniform vec4 u_edge_color;

void main()
{
	gl_FragColor = u_edge_color;
}

)";
