const char* model_skinned_vs = R"(

attribute vec4 position;
attribute vec3 normal;
attribute vec2 texcoord;
attribute vec4 blend_indices;
attribute vec4 blend_weights;

uniform mat4 u_projection;
uniform mat4 u_modelview;
uniform vec3 u_diffuse_material;
uniform vec3 u_ambient_material;
uniform vec3 u_specular_material;
uniform float u_shininess;
uniform mat3 u_normal_matrix;
uniform vec3 u_light_position;

uniform mat4 u_bone_matrix[60];

varying vec4 v_gouraud_dst;
varying vec2 v_texcoord;

int round2int(float f)
{
	return int(floor(f * 255.0 + 0.5));
}

void main()
{
	vec4 obj_pos = u_bone_matrix[round2int(blend_indices.x)] * position * blend_weights.x;
	obj_pos += u_bone_matrix[round2int(blend_indices.y)] * position * blend_weights.y;
	obj_pos += u_bone_matrix[round2int(blend_indices.z)] * position * blend_weights.z;
	obj_pos += u_bone_matrix[round2int(blend_indices.w)] * position * blend_weights.w;

	gl_Position = u_projection * u_modelview * obj_pos;

	vec3 eye_normal = u_normal_matrix * normal;
 	vec4 pos4 = u_modelview * position;
 	vec3 pos3 = pos4.xyz / pos4.w;
 	vec3 light_dir = normalize(u_light_position - pos3);
 	float diff = max(0.0, dot(eye_normal, light_dir));

 	vec4 gouraud_col = vec4(diff * u_diffuse_material, 1);
 	gouraud_col.rgb += u_ambient_material;
 	vec3 reflection = normalize(reflect(-light_dir, eye_normal));
 	float spec = max(0.0, dot(eye_normal, reflection));
 	spec = pow(spec, u_shininess);
 	gouraud_col.rgb += spec * u_specular_material;
	v_gouraud_dst = gouraud_col;

	v_texcoord = texcoord;
}

)";


const char* model_skinned_fs = R"(

uniform sampler2D u_texture0;

varying vec4 v_gouraud_dst;
varying vec2 v_texcoord;

void main()
{
	vec4 tex_map = texture2D(u_texture0, v_texcoord);
	vec4 tmp = v_gouraud_dst * tex_map;
	gl_FragColor = tmp;
}

)";
