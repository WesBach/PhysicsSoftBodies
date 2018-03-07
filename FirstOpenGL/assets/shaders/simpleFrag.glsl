// Fragment shader
#version 400

in vec4 color;					// was vec3
in vec3 vertNormal;
in vec3 vecWorldPosition;
in vec4 uvX2out;			// Added: UV 1 and 2 to fragment
							// UV #1 is .xy 
							// UV #2 is .zw
							//uniform vec4 diffuseColour;		// New variable

							// gl_FragColor is deprecated after version 120
							// Now we specify a specific variable.
							// If there is only 1, then GL will assume it's the colour 
out vec4 fragColourOut;


// The values our OBJECT material
uniform vec4 materialDiffuse;
//uniform vec4 materialAmbient;   		// 0.2
uniform float ambientToDiffuseRatio; 	// Maybe	// 0.2 or 0.3
uniform vec4 materialSpecular;  // rgb = colour of HIGHLIGHT only
								// w = shininess of the 
uniform vec3 eyePosition;	// Camera position

uniform bool bIsDebugWireFrameObject;

// Note: this CAN'T be an array (sorry). See 3D texture array
uniform sampler2D texSamp2D00;		// Represents a 2D image
uniform sampler2D texSamp2D01;		// Represents a 2D image
uniform sampler2D texSamp2D02;		// Represents a 2D image
uniform sampler2D texSamp2D03;		// Represents a 2D image
uniform sampler2D texSamp2D04;		// Represents a 2D image
uniform sampler2D texSamp2D05;		// Represents a 2D image
uniform sampler2D texSamp2D06;		// Represents a 2D image
uniform sampler2D texSamp2D07;		// Represents a 2D image
									// ... and so on...

uniform float texBlend00;
uniform float texBlend01;
uniform float texBlend02;
uniform float texBlend03;
uniform float texBlend04;
uniform float texBlend05;
uniform float texBlend06;
uniform float texBlend07;
// .... and so on... 

uniform bool isASkyBox;				// True samples the skybox texture
uniform samplerCube texSampCube00;
uniform samplerCube texSampCube01;
uniform samplerCube texSampCube02;
uniform samplerCube texSampCube03;
uniform float texCubeBlend00;
uniform float texCubeBlend01;
uniform float texCubeBlend02;
uniform float texCubeBlend03;

// For env. mapping (reflection and refraction)
uniform bool isReflectRefract;
uniform float reflectBlendRatio;		// How much reflection (0-1)
uniform float refractBlendRatio;		// How much refraction (0-1)
uniform float coefficientRefract; 		// coefficient of refraction 


uniform bool bIsSecondPass;			// True if the render is doing 2nd pass  
uniform sampler2D tex2ndPassSamp2D;		// Offscreen texture for 2nd pass
uniform float screenWidth;
uniform float screenHeight;

/*****************************************************/
struct sLightDesc {
	vec4 position;
	vec4 diffuse;
	vec4 ambient;
	vec4 specular;		// Colour (xyz), intensity (w)
	vec4 attenuation;	// x = constant, y = linear, z = quadratic
	vec4 direction;
	vec4 typeParams;	// x = type
						// 		0 = point
						// 		1 = directional
						// 		2 = spot
						// y = distance cut-off
						// z angle1, w = angle2		- only for spot
};

const int NUMBEROFLIGHTS = 20;
uniform sLightDesc myLight[NUMBEROFLIGHTS];

// This function has now been moved to the bottom of the file...
// Calculate the contribution of a light at a vertex
vec3 calcLightColour(in vec3 vecNormal,
	in vec3 vecWorldPosition,
	in int lightID,
	in vec3 matDiffuse,
	in vec4 matSpecular);
/*****************************************************/



void main()
{

	fragColourOut = vec4(0.0f, 0.0f, 0.0f, 1.0f);

	// Is this a 'debug' wireframe object, i.e. no lighting, just use diffuse
	if (bIsDebugWireFrameObject)
	{
		fragColourOut.rgb = materialDiffuse.rgb;			//gl_FragColor.rgb
		fragColourOut.a = materialDiffuse.a;				//gl_FragColor.a = 1.0f	
		fragColourOut.rgb += vec3(0.0f, 0.0f, 1.0f);

		fragColourOut * 1.5f;	// Room too bright
		return;		// Immediate return
	}

	if (bIsSecondPass)
	{
		vec2 textCoords = vec2(gl_FragCoord.x / screenWidth, gl_FragCoord.y / screenHeight);
		fragColourOut.rgb = texture(tex2ndPassSamp2D, textCoords).rgb;

		//		float average = (fragColourOut.r + fragColourOut.g + fragColourOut.b / 3.0f);
		//		fragColourOut.rgb = vec3(average, average, average);

		fragColourOut.a = 1.0f;
		return;
	}


	if (isASkyBox)
	{	// Sample from skybox texture and exit
		// I pass texture coords to 'sample' 
		//	returning a colour (at that point in the texture)
		// Note we are using the normals of our skybox object
		//	to determine the point on the inside of the box
		vec4 skyRGBA = texture(texSampCube00, vertNormal.xyz);

		fragColourOut = vec4(skyRGBA.rgb, 1.0f);		//gl_FragColor = skyRGBA;
														//		fragColourOut.rgb += vec3(0.0f, 1.0f, 0.0f);
		return;
	}

	// uniform bool isReflectRefract;
	// uniform float reflectBlendRatio;		// How much reflection (0-1)
	// uniform float refractBlendRatio;		// How much refraction (0-1)
	// uniform float coefficientRefract; 	// coefficient of refraction 
	if (isReflectRefract)
	{
		// Have "eyePosition" (camera eye) in WORLD space

		// reFLECTion value 
		vec3 vecReflectEyeToVertex = vecWorldPosition - eyePosition;
		vecReflectEyeToVertex = normalize(vecReflectEyeToVertex);
		vec3 vecReflect = reflect(vecReflectEyeToVertex, vertNormal.xyz);
		// Look up colour for reflection
		//		vec4 rgbReflection = texture( texSampCube00, vecReflect );
		vec4 rgbReflection = texture(texSampCube00, vertNormal.xyz);



		vec3 vecReFRACT_EyeToVertex = eyePosition - vecWorldPosition;
		vecReFRACT_EyeToVertex = normalize(vecReFRACT_EyeToVertex);
		vec3 vecRefract = refract(vecReFRACT_EyeToVertex, vertNormal.xyz,
			coefficientRefract);
		// Look up colour for reflection
		vec4 rgbRefraction = texture(texSampCube00, vecRefract);


		// Mix the two, based on how reflective the surface is
		fragColourOut = (rgbReflection * reflectBlendRatio) +
			(rgbRefraction * refractBlendRatio);

		//	fragColourOut.r = 1.0f;

		return;
	}

	vec3 matDiffuse = vec3(0.0f, 0.0f, 0.0f);

	// ****************************************************************/
	//uniform sampler2D myAmazingTexture00;
	vec2 theUVCoords = uvX2out.xy;		// use UV #1 of vertex

	vec4 texCol00 = texture(texSamp2D00, theUVCoords.xy);
	vec4 texCol01 = texture(texSamp2D01, theUVCoords.xy);

	//... and so on (to how many textures you are using)
	//	
	// use the blend value to combine textures
	//	vec3 matDiffuse = vec3(0.0f,1.0f,1.0f);
	//	vec3 matDiffuse = vec3(theUVCoords.xy, 0.0f);
	//	matDiffuse += texCol00.rgb;
	matDiffuse.rgb += 
		(texCol00.rgb * texBlend00) +
		(texCol01.rgb * texBlend01);;


	// We will look at specular or gloss maps later, 
	// 	but making the specular white is fine
	vec4 matSpecular = vec4(1.0f, 1.0f, 1.0f, 64.0f);

	// ****************************************************************/	
	for (int index = 0; index < NUMBEROFLIGHTS; index++)
	{
		// Old version, which used 'global' diffuse and specular
		//gl_FragColor.rgb += calcLightColour( vertNormal, vecWorldPosition, index );
		fragColourOut.rgb += calcLightColour(vertNormal,
			vecWorldPosition,
			index,
			matDiffuse,
			materialSpecular);
	}

	// Add the ambient here (AFTER the lighting)
	// We have materialAmbient, but ambient is often 
	//	just a percentage ratio of the diffuse
	vec3 ambientContribution = matDiffuse.rgb * ambientToDiffuseRatio;
	fragColourOut.rgb += ambientContribution.rgb;

	// Screen is so dim...
	fragColourOut *= 10.5f;	// 150% brighter

	// Copy object material diffuse to alpha
	fragColourOut.a = materialDiffuse.a;

	return;
}

// Inspired by Mike Bailey's Graphics Shader book
// (when you see "half-vector", that's eye space)
// Technically, a "per-pixel" or a form of Phong shading
// Calcualte the contribution of a light at a vertex
vec3 calcLightColour(in vec3 vecNormal,
	in vec3 vecWorldPosition,
	in int lightID,
	in vec3 matDiffuse, 	// ADDED
	in vec4 matSpecular)	// ADDED
{
vec3 colour = vec3( 0.0f, 0.0f, 0.0f );
	
	vec3 outDiffuse = vec3( 0.0f, 0.0f, 0.0f );
	vec3 outSpecular = vec3( 0.0f, 0.0f, 0.0f );
	

	// Distance between light and vertex (in world)
	vec3 vecToLight = myLight[lightID].position.xyz - vecWorldPosition;
	// How long is this vector?
	float lightDistance = length(vecToLight);
	// Short circuit distance
	if ( myLight[lightID].typeParams.y <  lightDistance )
	{
		return colour;
	}
	
	// The "normalized" vector to the light (don't care about distance, just direction)
	vec3 lightVector = normalize( vecToLight );
	
	// The normalized version of the vertex normal
	vecNormal = normalize(vecNormal);
	
	// How much diffuse light is being "reflected" off the surface 
	float diffFactor = max(0.0f, dot( lightVector, vecNormal ));
	
	
	outDiffuse.rgb = myLight[lightID].diffuse.rgb 		// Light contribution
	                 * matDiffuse.rgb				// Material contribution
					 * diffFactor;						// Factor based on direction

// Simple linear attenuation
//	float attenuation = 1.0f / lightDistance; 
	
	// From the "Shader" book article
	// 1.0 dividedBy ( constant 
	//                + linear * distance 
	//                + quadratic * distance * distance)
	// Our light description is: 
	// 	vec4 attenuation;	// x = constant, 
	//                         y = linear, 
	//                         z = quadratic
	float attenuation = 1.0f / 
	   ( myLight[lightID].attenuation.x // 0  
	   + myLight[lightID].attenuation.y * lightDistance			// Linear
	   + myLight[lightID].attenuation.z * lightDistance * lightDistance );	// Quad
	outDiffuse.rgb *= attenuation;

	
	// Vector from vertex to eye 
	// i.e. this makes the vector base effectively at zero.
	vec3 viewVector = normalize( eyePosition - vecWorldPosition );
	vec3 vecLightReflection = reflect( normalize(lightVector), vecNormal );
	
	float specularShininess = matSpecular.w;	// 64.0f
	vec3 specMatColour = matSpecular.rgb;		// vec3(1.0f, 1.0f, 1.0f);
	
	outSpecular.rgb = pow( max(0.0f, dot(viewVector,vecLightReflection)), 
	                  specularShininess)
			          * specMatColour
				      * myLight[lightID].specular.rgb;// // myLightSpecular;
				   
	outSpecular *= attenuation;
	
	
// For now, to simplify, eliminate the specular
//	colour = outDiffuse + outSpecular;
	colour = outDiffuse;
	
	// Now we have the colour if this was a point light 
	// See if it's a spot light...
	if ( myLight[lightID].typeParams.x == 2.0f ) 			// x = type
	{	// Then it's a spot light! 
		// 		0 = point
		// 		1 = directional
		// 		2 = spot
	    // z angle1, w = angle2		- only for spot
		
		// Vector from the vertex to the light... 
		vec3 vecVertexToLight = vecWorldPosition - myLight[lightID].position.xyz;
		// Normalize to unit length vector
		vec3 vecVtoLDirection = normalize(vecVertexToLight);
		
		float vertSpotAngle = max(0.0f, dot( vecVtoLDirection, myLight[lightID].direction.xyz ));
		// Is this withing the angle1?
		
//		float angleInsideCutCos = cos(myLight[lightID].typeParams.z);		// z angle1
//		float angleOutsideCutCos = cos(myLight[lightID].typeParams.w);		// z angle2
		float angleInsideCutCos = cos(myLight[lightID].typeParams.z);		// z angle1
		float angleOutsideCutCos = cos(myLight[lightID].typeParams.w);		// z angle2
		
		if ( vertSpotAngle <= angleOutsideCutCos )
		{	// NO, it's outside this angle1
			colour = vec3(0.0f, 0.0f, 0.0f );
		}
		else if ( (vertSpotAngle > angleOutsideCutCos ) && 
		          (vertSpotAngle <= angleInsideCutCos) )
		{	// inside the 'penumbra' region
			// Use smoothstep to get the gradual drop off in brightness
			float penRatio = smoothstep(angleOutsideCutCos, 
			                            angleInsideCutCos, 
										vertSpotAngle );          
			
			colour *= penRatio;
		}
//		else if ( vertSpotAngle <= angleInsideCutCos )
//		{
//			// Full brightness .... do nothing
//		}

//		colour.rgb*= 0.0001f;
//		colour.r = 1.0f;

	}//if ( typeParams.x
	
	
	return colour;
}// vec3 calcLightColour(...) 





