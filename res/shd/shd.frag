#version 420 core

uniform sampler2D sampler[gl_MaxTextureImageUnits];

flat in int instId;
flat in int samplerIndex;

in vec2 uv;

out vec4 color;

void main() {
	
	float textRed = texture(sampler[samplerIndex], uv).r;

	color = vec4(.01f * (instId + 1), textRed, .3, 1);

}