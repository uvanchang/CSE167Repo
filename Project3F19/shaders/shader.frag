#version 330 core
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

	float linear;
};

in vec3 normal;
in vec3 fragPos;

uniform vec3 viewPos;
uniform int normalColoring;
uniform int isLight;
uniform Material material;
uniform Light light;

// You can output many things. The first vec4 type output determines the color of the fragment
out vec4 fragColor;

void main()
{
	if (isLight == 1) {
		fragColor = vec4(vec3(light.ambient), 1.0);
	}
	else if (normalColoring == 0) {
		// ambient
		vec3 ambient = light.ambient * material.ambient;
  	
		// diffuse 
		vec3 norm = normalize(normal);
		vec3 lightDir = normalize(light.position - fragPos);
		float diff = max(dot(norm, lightDir), 0.0);
		vec3 diffuse = light.diffuse * (diff * material.diffuse);
    
		// specular
		vec3 viewDir = normalize(viewPos - fragPos);
		vec3 reflectDir = reflect(-lightDir, norm);  
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
		vec3 specular = light.specular * (spec * material.specular);  
        
		float distance    = length(light.position - fragPos);
		float attenuation = 1.0 / (light.linear * distance);

		diffuse *= attenuation;
		specular *= attenuation;

		vec3 result = ambient + diffuse + specular;
		fragColor = vec4(result, 1.0);
	}
	else {
		fragColor = vec4((normalize(normal) + vec3(1.0)) / vec3(2.0), 1.0);
	}
}