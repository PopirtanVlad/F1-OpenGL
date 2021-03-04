#version 410 core

#define NR_OF_POINT_LIGHTS 3

in vec3 fPosition;
in vec3 fNormal;
in vec2 fTexCoords;
in vec4 fPosEye;

out vec4 fColor;


//matrices
uniform mat4 model;
uniform mat4 view;
uniform mat3 normalMatrix;
//lighting
uniform vec3 lightDir;
uniform vec3 lightColor;
// textures
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;

//components
vec3 ambient;
float ambientStrength = 0.2f;
vec3 diffuse;
vec3 specular;
float specularStrength = 0.5f;

float constant = 1.0f;
float linear = 0.0045f;
float quadratic = 0.0075f;
uniform vec3 pointLightsColor[NR_OF_POINT_LIGHTS];
uniform vec3 pointLights[NR_OF_POINT_LIGHTS];
void computePointLight(vec3 lightPos, vec3 pointLightColor)
{
    vec3 viewDir = normalize(- fPosEye.xyz);
    vec3 lightDirN = normalize(lightPos - fPosition);
    //compute attenuation
    float dist = length(lightPos - fPosition);
    float att = 1.0f / (constant + linear * dist + quadratic * (dist * dist));
    
    //compute ambient light
    ambient += att * ambientStrength * lightColor;
    
    //compute diffuse light
    vec3 normal = normalize(fNormal);	
    diffuse += att * max(dot(normal, lightDirN), 0.0f) * pointLightColor;
    
    //compute specular light
    vec3 reflectLine = reflect(-lightDirN, normal);
    float specCoeff = pow(max(dot(viewDir, reflectLine), 0.0f), 32.0f);
    specular += att * specularStrength * specCoeff * lightColor;
}

void computeDirLight()
{
    //compute eye space coordinates
    vec4 fPosEye = view * model * vec4(fPosition, 1.0f);
    vec3 normalEye = normalize(normalMatrix * fNormal);

    //normalize light direction
    vec3 lightDirN = vec3(normalize(view * vec4(lightDir, 0.0f)));

    //compute view direction (in eye coordinates, the viewer is situated at the origin
    vec3 viewDir = normalize(- fPosEye.xyz);

    //compute ambient light
    ambient += ambientStrength * lightColor;

    //compute diffuse light
    diffuse += max(dot(normalEye, lightDirN), 0.0f) * lightColor;

    //compute specular light
    vec3 reflectDir = reflect(-lightDirN, fNormal);
    float specCoeff = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
    specular+= specularStrength * specCoeff * lightColor;
}

void main() 
{
    computeDirLight();
    for(int i=0;i<NR_OF_POINT_LIGHTS;i++){
        computePointLight(pointLights[i],pointLightsColor[i]);
    }
    vec3 color = min((ambient + diffuse) * texture(diffuseTexture, fTexCoords).rgb + specular * texture(specularTexture, fTexCoords).rgb, 1.0f);
    fColor = vec4(color, 1.0f);
}
