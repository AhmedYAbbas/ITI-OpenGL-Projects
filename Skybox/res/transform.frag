#version 330 core

out vec4 FragColor;

in vec3 worldPos;
in vec3 normal;
in vec2 texCoords;

uniform vec4 lightDirection;
uniform vec3 cameraPosition;

uniform float ambientCoeff;
uniform float diffuseCoeff;
uniform float specularCoeff;

uniform vec4 ambientColor;
uniform vec4 diffuseColor;
uniform vec4 specularColor;

uniform samplerCube skybox;
uniform sampler2D diffuse;
uniform sampler2D specular;

void main()
{
	vec3 I = normalize(worldPos - cameraPosition);
	vec3 R = reflect(I, normalize(normal));

	// To get Smooth/Phong Shading
	vec3 normalizedNormal = normalize(normal);

	// Remember to normalize stuff
	vec3 normalizeLightDirection = normalize(vec3(lightDirection));

	// Normal dot Light
	float ndotL = max(dot(normalizedNormal, normalizeLightDirection), 0.0); 

	vec4 skyboxMap = texture(skybox, R) * texture(specular, texCoords);

	// Light value
	vec4 lightValue = vec4(0.0, 0.0, 0.0, 0.0);

	// Ambient
	lightValue += ambientCoeff * ambientColor;

	// Diffuse
	lightValue += diffuseColor * diffuseCoeff * ndotL * texture(diffuse, texCoords);

	// Specular
	vec3 viewDir = normalize(cameraPosition - worldPos);
	vec3 reflectedRay = normalize( reflect(-normalizeLightDirection, normalizedNormal) );

	lightValue += specularColor * pow(max(dot(reflectedRay, viewDir) , 0.0), 32.0) * specularCoeff  * texture(specular, texCoords);

	FragColor = mix(skyboxMap, lightValue, 0.5f);
}