const char* scene_vs = R"(

attribute vec4 position;
attribute vec2 texcoord;

uniform mat4 u_projection;
uniform mat4 u_modelview;

varying vec2 v_texcoord;

void main()
{
	gl_Position = u_projection * u_modelview * position;
	v_texcoord = texcoord;
}

)";

const char* scene_fs = R"(

uniform sampler2D u_texture0;

varying vec2 v_texcoord;

void main()
{
	gl_FragColor = texture2D(u_texture0, v_texcoord);
}

)";
