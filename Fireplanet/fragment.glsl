#version 120

uniform float time; // Periodic time in the range [0, 1] with a period of 64s
uniform sampler2D texture; // Fire texture

varying vec2 coord; // Screen coordinates in the range [-1, 1]

float snoise(vec3 v); // Included from seperate GLSL file

const float RADIUS = .8; // Planet radius relative to window size

// Generates 2D turbulance from 3D coordinates
vec2 turbulance(vec3 coords)
{

	vec2 latlon = coords.xy;
	float time = coords.z;

	// These amplitude and frequency values have been decided through try, error and artistic freedom
	vec2 turb =  vec2(snoise(vec3(latlon, time)), snoise(vec3(latlon + vec2(.5, 0.), time)));
	turb += .5 * vec2(snoise(2. * vec3(latlon, time)), snoise(2. * vec3(latlon + vec2(.5, 0.), time)));
	turb += 1.5 * vec2(snoise(4. * vec3(latlon, 1.5*time)), snoise(4. * vec3(latlon + vec2(.5, 0.), 1.5*time)));
	turb += 2. * vec2(snoise(8. * vec3(latlon, 2.*time)), snoise(8. * vec3(latlon + vec2(.5, 0.), 2.*time)));
	turb += 2. * vec2(snoise(9. * vec3(latlon, 2.*time)), snoise(9. * vec3(latlon + vec2(.5, 0.), 2.*time)));
	turb += 1. * vec2(snoise(19. * vec3(latlon, 2.*time)), snoise(19. * vec3(latlon + vec2(.5, 0.), 2.*time)));
	turb += .7 * vec2(snoise(29. * vec3(latlon, 2.*time)), snoise(29. * vec3(latlon + vec2(.5, 0.), 2.*time)));
	turb /= 15.;

	return turb;
}

void main(void)
{
	vec2 center = vec2(0., 0.); // Planet center position
	vec2 planetCoord = coord - center; // Planet centric coordinates
	float planet = length(planetCoord);

	// Finds planet in screen space. Uses smoothstep to soften the countours. 
	float hit = 1. - smoothstep(RADIUS - .02, RADIUS, planet); // >0 if this fragment is part of planet

	// Calculate this fragment's planet position in 3D space
	vec3 planet3d = vec3(planetCoord, sqrt(RADIUS * RADIUS - planet * planet));
	planet3d = RADIUS * normalize(planet3d);

	// Calculates latitude and longitude on planet. Normalized to [0 - 1].
	vec2 latlon = vec2(dot(normalize(planet3d.xz), vec2(1., 0.)), dot(planet3d.yz, vec2(1., 0.)));
	latlon = acos(latlon) * .5 - .25;

	// Apply turbulance to the planet texture coordinates
	latlon += turbulance(vec3(latlon, time));

	// Fetch color from fire texture with the pertrubed coordinates
	vec4 fire = texture2D(texture, latlon);

	// Color emmitance from fire
	vec3 color = .7 * fire.rgb;

	// Add a directional light source from the (1,1,1) direction
	float light = dot(planet3d, normalize(vec3(1.)));

	// Output a mix of fire emmited light and soft directional light
	gl_FragColor = vec4(hit * (color + .5* light), 1.);
}
