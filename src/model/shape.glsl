const char* model_shape_vs = R"(

attribute vec4 position;
attribute vec3 normal;
attribute vec2 texcoord;
attribute vec4 blend_indices;
attribute vec4 blend_weights;

uniform mat4 u_projection;
uniform mat4 u_modelview;

uniform mat4 u_bone_matrix[60];

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
}

)";


const char* model_shape_fs = R"(

void main()
{
	gl_FragColor = vec4(1, 1, 0, 1);
}

)";
