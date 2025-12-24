#version 450 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

uniform sampler2D diffuseTexture;
uniform sampler2D shadowMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal)
{
    // 当我们在顶点着色器输出一个裁切空间顶点位置到gl_Position时，OpenGL自动进行一个透视除法，例如，将裁切空间坐标的范围[-w,w]转为[-1,1]，这要将x、y、z分量除以向量的w分量来实现。由于裁切空间的fragPosLightSpace并不会通过gl_Position传到片段着色器里，我们必须自己做透视除法
    // 下面过程首先通过除法进行统一，然后将结果 * 0.5 + 0.5 将[-1,1]范围的数据转换成对应的纹理位置[0, 1]数据，用于从深度纹理图片中提取对应值。
    //   同时因为来自深度贴图的深度范围是[0,1]，我们也打算使用projCoords从深度贴图中去采样，所以我们将NDC坐标范围变换为[0,1]。
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float bias = max(0.05 * (1 - dot(normal, normalize(lightPos))), 0.005);

    // PCF
    float shadow = 0.0f;
    // 此处的textureSize返回指定采样器纹理在0级mipmap的宽高向量，类型为vec2。取其倒数，即可得到单一一个纹理像素的大小，可以用来对纹理坐标进行偏移，从而确保每次都采样不同的深度值。本示例在每个投影坐标周围采样9个点来进行阴影判断，最终取平均值。
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

//shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

    if (projCoords.z > 1.0)
    {
        shadow = 0.0;
    }

    return shadow;
}

void main()
{           
    vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(0.3);
    // ambient
    vec3 ambient = 0.3 * lightColor;
    // diffuse
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;    
    // calculate shadow
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace, normal);
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    
    
    FragColor = vec4(lighting, 1.0);
}
