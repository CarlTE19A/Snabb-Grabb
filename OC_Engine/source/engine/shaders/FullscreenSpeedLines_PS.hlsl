#include "DeferredCommon.hlsli"
#include "pbr_functions.hlsli"
#define RADIUS 16.0
#define EDGE 0.3


#define minOffset .05
#define maxOffset .08
//#define maxOffsetBack .98
//#define minOffsetBack .55
#define simulationSpeed 0.9
#define elementsToDisplay 40.
#define maxDistFromCam 0.1
#define minDistFromCam 0.01


//float3 random3(float3 c)
//{
//    float j = 4096.0 * sin(dot(c, float3(17.0, 59.4, 15.0)));
//    float3 r;
//    r.z = frac(512.0 * j);
//    j *= .125;
//    r.x = frac(512.0 * j);
//    j *= .125;
//    r.y = frac(512.0 * j);
//    return r - 0.5;
//}

//float simplex3d(float3 p)
//{
//    float3 s = floor(p + dot(p, (float3)0.3333333));
//    float3 x = p - s + dot(s, (float3)0.1666667);
//    float3 e = step((float3)0.0, x - x.yzx);
//    float3 i1 = e * (1.0 - e.zxy);
//    float3 i2 = 1.0 - e.zxy * (1.0 - e);
//    float3 x1 = x - i1 + 0.1666667;
//    float3 x2 = x - i2 + 2.0  * 0.1666667;
//    float3 x3 = x - 1.0 + 3.0 * 0.1666667;
//    float4 w, d;
//    w.x = dot(x, x);
//    w.y = dot(x1, x1);
//    w.z = dot(x2, x2);
//    w.w = dot(x3, x3);
//    w = max(0.6 - w, 0.0);
//    d.x = dot(random3(s), x);
//    d.y = dot(random3(s + i1), x1);
//    d.z = dot(random3(s + i2), x2);
//    d.w = dot(random3(s + 1.0), x3);
//    w *= w;
//    w *= w;
//    d *= w;
//    return dot(d, -(float4)52.0);
//}

//Noise, 2 in 1 out
float N21(float2 p)
{
    p = frac(p * float2(233.34, 851.73));
    p += dot(p, p + 23.45);
    return frac(p.x * p.y);
}

//Noise, 2 in 2 out
float2 N22(float2 p)
{
    float n = N21(p);
    return float2(n, (N21(p + n)));
}


//Creates a line between a and b, from texture coord at p
float DistLine(float2 p, float2 a, float2 b)
{
    //Vector from texture coord to line start
    float2 pa = a - p;
    //Vector from texture coord to line end
    float2 ba = a - b;
    float t = clamp(dot(pa, ba) / dot(ba, ba), 0., 1.);
    return length(pa - ba * t);
}

//p - currentPoint, a - startPoint, b - endPoint
float LineVec(float2 p, float2 a, float2 b)
{
    //Create a line from a to b, using uv coords at p
    float d = DistLine(p, a, b);

    //Multiplicative inverse distance to start point     
    float pFactor = 1. / length(p - a);
	
    //Calculate the size and deformation of the line
    float mask = smoothstep(.03, 0.02, d * pFactor);
    return mask;
}

float SpeedLine(float2 uv, float2 direction, float depth)
{
    return LineVec(uv - direction * depth, (float2)0, direction);
}

float RemapValueRange(float inputValue, float iLow, float iHigh, float tLow, float tHigh)
{
    return ((inputValue - iLow) / (iHigh - iLow)) * (tHigh - tLow) + tLow;
}

PixelOutput main(FullscreenVertexToPixel input)
{
    PixelOutput result;
    if (speedLinesSpeed <= 0.01f) { discard; }
    
    float2 UV = input.Position.xy / resolution.xy;
    float timer = time;
    
    float3 CameraRight = normalize(cross(CameraForward, float3(0, 1, 0)));
    float3 direction = CameraForward - (speedLinesDirection);
    float directionH = RemapValueRange(dot(CameraRight, speedLinesDirection), -speedLinesSpeed, speedLinesSpeed, -.9f, .9f);
    //float directionF = dot(CameraForward, speedLinesDirection);

    UV.y -= direction.y * 0.4f;
    UV.x += directionH * 0.5f;

    UV -= .5; //Sets origin in the middle

    //bool isReversed = false;
    //if (directionF < 0)
    //{
    //    isReversed = true;
    //}
    
    //UV.x *= resolution.x / resolution.y; //"Sqaures" the uv input 

    //Creates a time variable, adjusted for the sim speed
    float t = (timer * simulationSpeed);
    //Creates an empty mask to contain the effect
    float vecMask = 0;

	
    //Loop to draw every line.
    for (float u = 0.; u < 1.; u += 1. / elementsToDisplay)
    {
        //Gets time decimals, between 0.0 and 1.0
        float z = frac(t + u);
								
        //Creates depth
        float size = lerp(maxDistFromCam, minDistFromCam, z);
        //fades in the lines, accoring to the time they have existed / depth of the object
        float fade = smoothstep(0.2, .7, z);
				
        //Generate a random vector. Subtract .5 to make it between -.5 & .5
        float2 randomPoint = normalize(N22(float2(1.5 * u, 1.5 * -u)) - .5);

		//Choose a spawn point for the lines, by using the direction they should spawn in
		//multiplied by a random factor, between two offset values
        float2 spawnPosition = randomPoint * lerp(minOffset, maxOffset, z);

        //Calculates the uv offset (spawn distance from center)
        float2 uvOffset = UV + spawnPosition;
        //Calculates the direction the lines should fly
        float2 flyDirection = -spawnPosition;

        
        //if(isReversed)
        //{
        //    z = frac(t - u);
        //    spawnPosition = randomPoint * lerp(minOffsetBack, maxOffsetBack, z);
        //    uvOffset = UV + spawnPosition;
        //    flyDirection = spawnPosition;
        //    size = lerp(minDistFromCam, maxDistFromCam, z);
        //    fade = smoothstep(0.2, .7, z);
        //}
        
		//Runs the function to create the lines, with the calculated values
        float currentMask = SpeedLine(uvOffset * size, flyDirection, z) * fade;
        //Applies the generated line to the line mask
        vecMask += currentMask;
    }
    
    
    //// fast lines on the sides
    //float scale = 50.0;
    ////float2 uv = (input.Position.xy * 2. - resolution.xy) / resolution.y * 0.5;
    //float2 uv = float2((input.Position.x * 2.f - resolution.x), (input.Position.y * 2.f - resolution.y)) / resolution.y * 0.5;
    
    //float2 p = float2(resolution.x / resolution.y, 0.5) + normalize(uv) * min(length(uv), 0.05);
    //float3 p3 = scale * 0.20 * float3(p.xy, 0) + float3(0, 0, timer * 0.05); // add player speed here timer * 0.05
    
    //float noise = simplex3d(p3 * 64.0) * 0.5 + 0.5;
    //float dist = abs(clamp(length(uv) / RADIUS * 0.7, 0.0, 1.0) * (1 / noise) * 2. - 1.); // *2 strenght // RADIUS * speed
    //float stepped = smoothstep(EDGE - .5, EDGE + .5, noise * (1.0 - pow(dist, 4.0)));
    
    //float final = smoothstep(EDGE - 0.05, EDGE + 0.15, noise * stepped);
    //result.color = float4(float3(1.0, 1.0, 1.0) * final, final);
    
    
    
    result.color = (float4) vecMask;
    return result;
}