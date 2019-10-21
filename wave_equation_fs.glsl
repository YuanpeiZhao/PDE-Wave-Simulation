#version 400

uniform sampler2D tex;

out vec4 fragcolor;          

in vec3 Normal;  
in vec3 FragPos;  
  
vec3 lightPos = vec3(0.0f, 0.0f, 1.0f); 
vec3 lightColor = vec3(1.0f, 1.0f, 1.0f); 
vec3 objectColor = vec3(0.5f, 0.5f, 0.8f);
vec3 viewPos = vec3(0.0f, -1.5f, 1.5f);
     
void main(void)
{   
   // ambient
    float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * lightColor;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(FragPos - lightPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

	// specular
    float specularStrength = 0.5f;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;  
        
    vec3 result = (ambient + diffuse + specular) * objectColor;

    fragcolor = vec4(result, 1.0f);
}




















