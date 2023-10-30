#version 330 core

out vec4 fColor;

in vec2 fTexCoords;

uniform sampler2D render;

const float offset = 1.0 / 350.0; 
const float streakOffset = 1.0 / 150; 

void main()
{
    vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // top-left
        vec2( 0.0f,    offset), // top-center
        vec2( offset,  offset), // top-right
        vec2(-offset,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( offset,  0.0f),   // center-right
        vec2(-offset, -offset), // bottom-left
        vec2( 0.0f,   -offset), // bottom-center
        vec2( offset, -offset)  // bottom-right    
    );

    float kernel[9] = float[](
		1.0 / 16, 2.0 / 16, 1.0 / 16,
		2.0 / 16, 4.0 / 16, 2.0 / 16,
		1.0 / 16, 2.0 / 16, 1.0 / 16  
	);



	float fBrightness = 1;
	vec2 texCoords = fTexCoords;
	/* if (texCoords.x > 0.5) {
		texCoords.x += 20*pow((texCoords.x-0.5), 8);
		fBrightness -= 40*pow((texCoords.x-0.5), 8);
	}
	else {
		texCoords.x -= 20*pow((texCoords.x-0.5), 8);
		fBrightness -= 40*pow((texCoords.x-0.5), 8);
	} */

	// BRANCHLESS

	texCoords.x += int(texCoords.x > 0.5) * (20*pow((texCoords.x-0.5), 8));
	texCoords.x -= int(!(texCoords.x > 0.5)) * (20*pow((texCoords.x-0.5), 8));
	fBrightness -= (40*pow((texCoords.x-0.5), 8));

	//



	/* if (texCoords.y > 0.5) {
		texCoords.y += 20*pow((texCoords.y-0.5), 8);
		fBrightness -= 40*pow((texCoords.y-0.5), 8);
	}
	else {
		texCoords.y -= 20*pow((texCoords.y-0.5), 8);
		fBrightness -= 40*pow((texCoords.y-0.5), 8);
	} */

	// BRANCHLESS

	texCoords.y += int(texCoords.y > 0.5) * (20*pow((texCoords.y-0.5), 8));
	texCoords.y -= int(!(texCoords.y > 0.5)) * (20*pow((texCoords.y-0.5), 8));
	fBrightness -= (40*pow((texCoords.y-0.5), 8));

	//



	const float streakLength = 50;
	vec3 colorStreaks[100];
	for (int i=0; i < streakLength; i++) {
		colorStreaks[i*2] = vec3(texture(render, texCoords + vec2(streakOffset/2*i,  0.0f)));
		colorStreaks[(i*2)+1] = vec3(texture(render, texCoords - vec2(streakOffset/2*i,  0.0f)));
		// colorStreaks[i] = vec3(col + vec3(offset/2*i,  0.0f, 0.0f));
	}

	float streakBrightness[50];
	for (int i=0; i < streakLength; i++) {
		streakBrightness[i] = pow(30.0+pow(i,7.00)/20, 0.2);
	}

	vec3 colorStreakVal = vec3(0.0);
	for (int i=0; i < streakLength; i++) {
		colorStreakVal += colorStreaks[i] / streakBrightness[i];
	}



    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(render, texCoords + offsets[i]));
    }
    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++)
        col += sampleTex[i] * kernel[i];



	float power = 0.8;

	vec4 outputTexture = texture(render, texCoords);
	outputTexture = vec4(pow(outputTexture.x, power), pow(outputTexture.y, power), pow(outputTexture.z, power), 1.0);
	vec4 outputCol = vec4(pow(col.x*2, power), pow(col.y*2, power), pow(col.z*2, power), 1.0);

	/* if (texCoords.x > 1 || texCoords.x < 0 || texCoords.y > 1 || texCoords.y < 0) {
		fColor = vec4(0.0, 0.0, 0.0, 1.0);
	}
	else {
    	fColor = (outputTexture + outputCol + vec4(colorStreakVal, 1.0)) * pow(fBrightness, 10);
	} */

	// BRANCHLESS

	fColor = (int(texCoords.x > 1 || texCoords.x < 0 || texCoords.y > 1 || texCoords.y < 0) * vec4(0.0, 0.0, 0.0, 1.0)) + (int(!(texCoords.x > 1 || texCoords.x < 0 || texCoords.y > 1 || texCoords.y < 0)) * (outputTexture + outputCol + vec4(colorStreakVal, 1.0)) * pow(fBrightness, 10));

	//
}  