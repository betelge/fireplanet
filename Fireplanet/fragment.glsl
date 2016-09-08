#version 120

uniform float time;
uniform sampler2D texture;

in vec2 coord;

float snoise(vec3 v); // Included from seperate GLSL file

const float RADIUS = .8; // Planet radius

// Generates 2D turbulance from 3D coordinates
vec2 turbulance(vec3 coords)
{

	vec2 latlon = coords.xy;

	float time = coords.z;

	vec2 turb =  vec2(snoise(vec3(latlon, time)), snoise(vec3(latlon + vec2(.5, 0.), time)));
	turb += .5 * vec2(snoise(2. * vec3(latlon, time)), snoise(2. * vec3(latlon + vec2(.5, 0.), time)));
	turb += 1.5 * vec2(snoise(4. * vec3(latlon, 1.5*time)), snoise(4. * vec3(latlon + vec2(.5, 0.), 1.5*time)));
	turb += 2. * vec2(snoise(8. * vec3(latlon, 2.*time)), snoise(8. * vec3(latlon + vec2(.5, 0.), 2.*time)));
	turb += 2. * vec2(snoise(9. * vec3(latlon, 2.*time)), snoise(9. * vec3(latlon + vec2(.5, 0.), 2.*time)));
	turb /= 12.;

	return turb;
}

void main(void)
{
	vec2 center = vec2(0., 0.); // Planet center position
	vec2 planetCoord = coord - center;
	float planet = length(planetCoord);

	float hit = 1. - smoothstep(RADIUS - .02, RADIUS, planet); // >0 if this fragment is part of planet

	// We calculate this fragment on planet position in 3D space
	vec3 planet3d = vec3(planetCoord, sqrt(RADIUS * RADIUS - planet * planet));
	planet3d = RADIUS * normalize(planet3d);

	vec2 latlon = vec2(dot(normalize(planet3d.xz), vec2(1., 0.)), dot(planet3d.yz, vec2(1., 0.)));
	latlon = latlon * .5 + .5;

	latlon += turbulance(vec3(latlon, time));

	vec4 fire = texture2D(texture, latlon);

	// Color emmitance from fire
	vec3 color = .7 * fire.rgb;

	// Add a light source from the (1,1,1) direction
	float light = dot(planet3d, normalize(vec3(1.)));

	gl_FragColor = vec4(hit * (color + .5* light), 1.);// vec4(snoise(5.*planet3d), hit*fire., 1.);
}
