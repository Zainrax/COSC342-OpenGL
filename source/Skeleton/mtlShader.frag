#version 330 core

// Interpolated values from the vertex shaders
in vec2 UV;
in vec3 posWorldspace;
in vec3 normalCameraspace; //normal in camera space
in vec3 eyeDirectionCameraspace; // eye direction in camera space
in vec3 lightDirectionCameraspace; // light direction in camera space

// Ouput data
out vec4 color;

// Values that stay constant for the whole mesh.
uniform sampler2D myTextureSampler;
uniform mat4 MV;
uniform vec3 lightPosWorldspace;

const float ns = 6.0; //specular exponent

// Light emission properties
const vec3 ambientLightColor = vec3(1.0);
const vec3 diffuseLightColor = vec3(1.0);
const vec3 specularLightColor = vec3(1.0);

uniform vec4 diffuseColor;
uniform vec4 ambientColor;
uniform vec4 specularColor;

uniform float opacity;
uniform float shininess;
uniform float renderMode;

const int levels = 5;
const float scaleFactor = 1.0 / levels;

//Based on https://en.wikipedia.org/wiki/Relative_luminance.
vec4 BlackWhiteFilter(in vec4 color){
    float r = 0.2126;
    float g = 0.7152;
    float b = 0.0722;
    return vec4(
    clamp(color.r * r + color.g * g + color.b * b, 0.0, 1.0)
    , clamp(color.r * r + color.g * g + color.b * b, 0.0, 1.0)
    , clamp(color.r * r + color.g * g + color.b * b, 0.0, 1.0)
    , color.a
    );
}

vec4 ToonFilter(in vec4 color){
    float r = 1.25;
    float g = 1.05;
    float b = 1.2;
    return vec4(
    clamp(color.r * r, 0.0, 1.0)
    , clamp(color.g * g, 0.0, 1.0)
    , clamp(color.b * b, 0.0, 1.0)
    , color.a
    );
}

void main(){

    // Material properties
    vec3 textureVal = texture( myTextureSampler, UV ).rgb;  //texture map will be used for diffuse and ambient texture map
    // We now work in camera space
    // Normal of the computed fragment, in camera space
    vec3 N = normalize( normalCameraspace );
    // Direction of the light (from the fragment to the light) in camera space
    vec3 L = normalize( lightDirectionCameraspace );

    // We use the cosine of the angle theta between the normal and the light direction to compute the diffuse component.
    // The cosine is clamped to contrain it between 0 and 1
    //  - light is at the vertical of the triangle -> 1
    //  - light is perpendicular to the triangle -> 0
    //  - light is behind the triangle -> 0
    float cosTheta = clamp( dot( N,L), 0,1 );

    // Eye vector (towards the camera)
    vec3 E = normalize(eyeDirectionCameraspace);
    // Direction in which the triangle reflects the light
    vec3 R = reflect(-L,N);

    // Cosine of the angle between the Eye vector and the Reflect vector,
    // The cosine is clamped to contrain it between 0 and 1 to avoid negative values
    //  - Looking into the reflection -> 1
    //  - Looking elsewhere -> < 1
    vec3 H = normalize(L+E);
    // Blinn-Phong
    float cosAlpha = clamp( dot( H,N ), 0,1 );


    vec3 diffuseComponent = diffuseColor.rgb * diffuseLightColor * textureVal * cosTheta;
    vec3 ambientComponent = ambientColor.rgb * ambientLightColor * textureVal;
    vec3 specularComponent = specularColor.rgb * specularLightColor  * pow(cosAlpha, shininess);

    //Toon but flooring the light at multiple levels.
    if(renderMode == 2){
        diffuseComponent = diffuseColor.rgb * diffuseLightColor * textureVal * floor(cosTheta * levels) * scaleFactor;
    }

    color.rgb =
    // Ambient : simulates indirect lighting
    ambientComponent.rgb +
    // Diffuse : "color" of the object
    diffuseComponent.rgb +
    // Specular : reflective highlight, like a mirror
    specularComponent.rgb;

    color.a = opacity;

    //black and white filter.
    if(renderMode == 1){
        color = BlackWhiteFilter(color);
    }






}