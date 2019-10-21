#version 400

out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;  
  
vec3 lightPos = vec3(0.0f, 0.0f, 1.0f); ; 
vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
vec3 objectColor = vec3(1.0f, 1.0f, 1.0f);

void main()
{
    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
            
    vec3 result = (ambient + diffuse) * objectColor;
    FragColor = vec4(result, 1.0);
} 


















