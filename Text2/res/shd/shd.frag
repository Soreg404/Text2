#version 420 core

uniform sampler2D sampler[gl_MaxTextureImageUnits];

flat in int instId;
flat in int samplerIndex;

in vec2 uv;

out vec4 color;

void main() {
	
	float textRed = texture(sampler[samplerIndex], uv).r;
	float smpl = float(samplerIndex) / 6.f;

	color = vec4(.04f * (instId + 1), smpl, .6, 1);

}