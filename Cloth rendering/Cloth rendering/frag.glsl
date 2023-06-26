#version 460 core

in vec2 texCoord;
out vec4 fragColor;
uniform sampler2D tex;
void main(void)
{
	// Set the color of this fragment to the interpolated color
	// value computed by the rasterizer.

	fragColor = texture(tex, vec2(texCoord.x, texCoord.y));
}
