#version 420 core

layout(location = 0)in ivec2 box;
layout(location = 1)in ivec2 pos;
layout(location = 2)in ivec2 size;
layout(location = 3)in int boundUnit;

flat out int instId;
flat out int samplerIndex;

out vec2 uv;

uniform mat4 projection = mat4(1);

void main() {
	
	int posX = pos.x, posY = pos.y;
	switch(gl_VertexID % 4) {
		case 0: posX += size.x + 10; uv = vec2(1, 0); break;
		case 1: uv = vec2(0, 0); break;
		case 2: posY -= size.y + 10; uv = vec2(0, 1); break;
		case 3: posX += size.x + 10; posY -= size.y + 10; uv = vec2(1, 1); break;
	}

	gl_Position = projection * vec4(posX + 200, posY + 200, -1, 1);

	instId = gl_InstanceID;
	samplerIndex = boundUnit;

}
