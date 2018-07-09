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

uniform float u_brightness;

varying vec2 v_texcoord;

void main()
{
	gl_FragColor = texture2D(u_texture0, v_texcoord);

    gl_FragColor = vec4(vec3(u_brightness / 2.0 * gl_FragColor), gl_FragColor.a);
    gl_FragColor = clamp(2.0 * gl_FragColor, 0.0, 1.0);
//  gl_FragColor.a = Alpha;
}

)";
