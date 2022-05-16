#version 330

uniform sampler2D textureSampler;
uniform sampler2D additionalSampler;
uniform sampler2D normalSampler;
uniform sampler2D roughSampler; // DUDV!!!!!
uniform float t;
uniform float waveStrength;
uniform float layer;

uniform bool glass;

in vec2 texCoord;
in vec4 clipSpace;
out vec4 fragmentColor;

void main()
{
    vec4 res = texture(textureSampler, texCoord);

    if(glass == false)
    {
        vec3 ndc = (clipSpace.xyz / clipSpace.w)/ 2.0 + 0.1;

        vec2 reflectTexCoords = vec2(ndc.x, -ndc.y);
        vec2 refractTexCoords = vec2(ndc.x, ndc.y);

        vec2 distorsion = (texture(roughSampler, vec2(texCoord.x - t, texCoord.y + t)).rg * 2.0 - 1.0) * (t + waveStrength);
        vec2 distorsion2 = normalize(texture(normalSampler, vec2(texCoord.x + t, texCoord.y - t)).rg * 2.0 - 1.0) * (t * waveStrength);

        refractTexCoords += distorsion;
        refractTexCoords += distorsion2;

        reflectTexCoords += distorsion;
        reflectTexCoords += distorsion2;

        vec4 reflectColor = texture(textureSampler, reflectTexCoords);
        vec4 refractionColor = texture(normalSampler, refractTexCoords);

        res = mix(reflectColor, refractionColor, layer);
    }

    res.a = 0.8;
	fragmentColor = res;
}
