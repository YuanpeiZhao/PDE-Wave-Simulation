#version 400

in vec2 TexCoords;

uniform sampler2D tex;
uniform vec2 peak0_pos;
uniform int initialized;
uniform int cur_frame;
uniform float c;
uniform float miu;
uniform float d;
uniform float time;
uniform int peak_pattern;

vec2 islandPos = vec2(0.5f, 0.2f);
vec2 islandSize = vec2(0.4f, 0.1f);

out vec4 fragcolor;    

bool inPeak()
{
	if(peak_pattern == 0)
	{
		if(abs(TexCoords.x - peak0_pos.x) <= 0.2f && abs(TexCoords.y - peak0_pos.y) <= 0.2f)
		{
			return true;
		}
		return false;
	}

	if(distance(TexCoords, peak0_pos) < 0.2f)
	{
		return true;
	}
	return false;
}

float decode(float color)
{
	return (color - 0.5f) * 2.0f;
}

float incode(float height)
{
	return height / 2.0f + 0.5f;
}

vec3 compute()
{
	if(initialized == 0)
	{
		if(inPeak())
		{
			return vec3(0.5f, 0.5f, 0.5f);
		}
		else
		{
			return vec3(0.0f, 0.0f, 0.0f);
		}
	}
	else
	{
		vec3 result = texture2D(tex, TexCoords).xyz;

		if(abs(TexCoords.x - islandPos.x) < islandSize.x / 2 && abs(TexCoords.y - islandPos.y) < islandSize.y / 2)
		{
			return vec3(result.xy, 0.0f);
		}

		float z_cur = decode(texture2D(tex, TexCoords)[cur_frame]);
		float z_pre1_l = decode(texture2D(tex, TexCoords - vec2(d, 0.0f))[(cur_frame+2)%3]);
		float z_pre1_r = decode(texture2D(tex, TexCoords + vec2(d, 0.0f))[(cur_frame+2)%3]);
		float z_pre1_u = decode(texture2D(tex, TexCoords - vec2(0.0f, d))[(cur_frame+2)%3]);
		float z_pre1_d = decode(texture2D(tex, TexCoords + vec2(0.0f, d))[(cur_frame+2)%3]);
		float z_pre1 = decode(texture2D(tex, TexCoords)[(cur_frame+2)%3]);
		float z_pre2 = decode(texture2D(tex, TexCoords)[(cur_frame+1)%3]);

		float A = (4.0f - 8.0f * c * c * time * time / d / d) / (2.0f + miu * time);
		float B = (miu * time - 2.0f) / (miu * time + 2.0f);
		float C = 2.0f * c * c * time * time / d / d / (miu * time + 2.0f);

		z_cur = A * z_pre1 + B * z_pre2 + C * (z_pre1_l + z_pre1_r + z_pre1_d + z_pre1_u);

		result[cur_frame] = incode(z_cur);
		return result;
	}
}
void main(void)
{   
    fragcolor = vec4(compute(), 1.0f);
}




















