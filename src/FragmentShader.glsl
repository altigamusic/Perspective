uniform sampler2D _tex0;
uniform float _s0, _s1, _s2, _s3, _s4, _s5;
uniform float _t;
uniform int scene;
uniform vec3 _cp, _ct, _pp, _pt;
out vec4 fragColor;

//GLOBAL VARIABLES
const vec2 ep = vec2(.00035, -.00035);        //ep = epsilon = offset number for normals calculation
const float near = 0.0001;
const float far = 100.;                       //far = far plane = how far we march till we stop

const float MAT_BLUE = 1.;
const float MAT_YELLOW = 2.;
const float MAT_RED = 3.;
const float MAT_WHITE = 4.;
const float MAT_BLACK = 5.;

//PRIMITIVE FUNCTIONS
float box(vec3 p, vec3 r) {
    p = abs(p) - r;
    return max(max(p.x, p.y), p.z);
}
float box(vec2 p, vec2 s) {
    vec2 q = abs(p) - s;
    return length(max(q, 0.)) + min(max(q.x, q.y), 0.);
}

vec2 minX(vec2 a, vec2 b) {
    return a.x < b.x ? a : b;
}
mat2 rot(float angle) {
    float s = sin(angle), c = cos(angle);
    return mat2(c, -s, s, c);
}

const mat2 m = mat2(1.6, 1.2, -1.2, 1.6);

vec2 hash(vec2 p) {
    p = vec2(dot(p, vec2(127.1, 311.7)), dot(p, vec2(269.5, 183.3)));
    return -1.0 + 2.0 * fract(sin(p) * 43758.5453123);
}

float noise(in vec2 p) {
    const float K1 = 0.366025404; // (sqrt(3)-1)/2;
    const float K2 = 0.211324865; // (3-sqrt(3))/6;
    vec2 i = floor(p + (p.x + p.y) * K1), a = p - i + (i.x + i.y) * K2, o = (a.x > a.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0), //vec2 of = 0.5 + 0.5*vec2(sign(a.x-a.y), sign(a.y-a.x));
    b = a - o + K2, c = a - 1.0 + 2.0 * K2;
    vec3 h = max(0.5 - vec3(dot(a, a), dot(b, b), dot(c, c)), 0.0), n = h * h * h * h * vec3(dot(a, hash(i + 0.0)), dot(b, hash(i + o)), dot(c, hash(i + 1.0)));
    return dot(n, vec3(70.0));
}

vec4 permute(vec4 x) {
    return mod(((x * 34.0) + 1.0) * x, 289.0);
}
vec4 taylorInvSqrt(vec4 r) {
    return 1.79284291400159 - 0.85373472095314 * r;
}
vec3 fade(vec3 t) {
    return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
}

float cnoise(vec3 P) {
    vec3 Pi0 = floor(P); // Integer part for indexing
    vec3 Pi1 = Pi0 + vec3(1.0); // Integer part + 1
    Pi0 = mod(Pi0, 289.0);
    Pi1 = mod(Pi1, 289.0);
    vec3 Pf0 = fract(P); // Fractional part for interpolation
    vec3 Pf1 = Pf0 - vec3(1.0); // Fractional part - 1.0
    vec4 ix = vec4(Pi0.x, Pi1.x, Pi0.x, Pi1.x);
    vec4 iy = vec4(Pi0.yy, Pi1.yy);
    vec4 iz0 = Pi0.zzzz;
    vec4 iz1 = Pi1.zzzz;

    vec4 ixy = permute(permute(ix) + iy);
    vec4 ixy0 = permute(ixy + iz0);
    vec4 ixy1 = permute(ixy + iz1);

    vec4 gx0 = ixy0 / 7.0;
    vec4 gy0 = fract(floor(gx0) / 7.0) - 0.5;
    gx0 = fract(gx0);
    vec4 gz0 = vec4(0.5) - abs(gx0) - abs(gy0);
    vec4 sz0 = step(gz0, vec4(0.0));
    gx0 -= sz0 * (step(0.0, gx0) - 0.5);
    gy0 -= sz0 * (step(0.0, gy0) - 0.5);

    vec4 gx1 = ixy1 / 7.0;
    vec4 gy1 = fract(floor(gx1) / 7.0) - 0.5;
    gx1 = fract(gx1);
    vec4 gz1 = vec4(0.5) - abs(gx1) - abs(gy1);
    vec4 sz1 = step(gz1, vec4(0.0));
    gx1 -= sz1 * (step(0.0, gx1) - 0.5);
    gy1 -= sz1 * (step(0.0, gy1) - 0.5);

    vec3 g000 = vec3(gx0.x, gy0.x, gz0.x);
    vec3 g100 = vec3(gx0.y, gy0.y, gz0.y);
    vec3 g010 = vec3(gx0.z, gy0.z, gz0.z);
    vec3 g110 = vec3(gx0.w, gy0.w, gz0.w);
    vec3 g001 = vec3(gx1.x, gy1.x, gz1.x);
    vec3 g101 = vec3(gx1.y, gy1.y, gz1.y);
    vec3 g011 = vec3(gx1.z, gy1.z, gz1.z);
    vec3 g111 = vec3(gx1.w, gy1.w, gz1.w);

    vec4 norm0 = taylorInvSqrt(vec4(dot(g000, g000), dot(g010, g010), dot(g100, g100), dot(g110, g110)));
    g000 *= norm0.x;
    g010 *= norm0.y;
    g100 *= norm0.z;
    g110 *= norm0.w;
    vec4 norm1 = taylorInvSqrt(vec4(dot(g001, g001), dot(g011, g011), dot(g101, g101), dot(g111, g111)));
    g001 *= norm1.x;
    g011 *= norm1.y;
    g101 *= norm1.z;
    g111 *= norm1.w;

    float n000 = dot(g000, Pf0);
    float n100 = dot(g100, vec3(Pf1.x, Pf0.yz));
    float n010 = dot(g010, vec3(Pf0.x, Pf1.y, Pf0.z));
    float n110 = dot(g110, vec3(Pf1.xy, Pf0.z));
    float n001 = dot(g001, vec3(Pf0.xy, Pf1.z));
    float n101 = dot(g101, vec3(Pf1.x, Pf0.y, Pf1.z));
    float n011 = dot(g011, vec3(Pf0.x, Pf1.yz));
    float n111 = dot(g111, Pf1);

    vec3 fade_xyz = fade(Pf0);
    vec4 n_z = mix(vec4(n000, n100, n010, n110), vec4(n001, n101, n011, n111), fade_xyz.z);
    vec2 n_yz = mix(n_z.xy, n_z.zw, fade_xyz.y);
    float n_xyz = mix(n_yz.x, n_yz.y, fade_xyz.x);
    return 2.2 * n_xyz;
}

float fbm(vec2 n) {
    float total = 0.0, amplitude = 0.1;
    for(int i = 0; i < 7; i++) {
        total += noise(n) * amplitude;
        n = m * n;
        amplitude *= 0.4;
    }
    return total;
}

float h = 2.;
float th = .2; // Thickness

float sdfP(vec2 p) {
    float co = .2; // Circle offset
    float w = .5; // P offset width

    float stem = box(p + vec2(th, 0), vec2(th, h));
    p.y -= 1. - th - co;
    vec2 q = p - vec2(w, 0);
    float circle = max(-q.x, abs(length(q) - 1. - co) - th);
    p.y = abs(p.y) - 1.;
    float stem2 = box(p - vec2(w / 2., th), vec2(w / 2., th));

    return min(min(stem, stem2), circle);
}

float sdfR(vec2 p) {
    float co = .2; // Circle offset (from P)
    float a = -.7;
    float q = .9;
    float pPart = sdfP(p);
    p += vec2(-.5, 1. - th - co);
    p *= rot(a);
    float stem = box(p + vec2(0, q), vec2(th, q));

    return min(pPart, stem);
}

float sdfE(vec2 p) {
    p.y = abs(p.y);

    float w = .85;

    float stem = box(p, vec2(th, h));
    float finger1 = box(p - vec2(w, 0), vec2(w, th));
    float finger2 = box(p - vec2(w, h - th), vec2(w, th));

    return min(stem, min(finger1, finger2));
}

float sdfS(vec2 p) {
    float a = .5; // angle
    float w = .3; // width between semicircles
    float s = .9; // scale
    p /= s;
    p *= rot(a);

    vec2 offset = vec2(w, -1);
    vec2 q = p - offset;
    float circle1 = max(-q.x, abs(length(q) - 1.) - th);
    q = p + offset;
    float circle2 = max(q.x, abs(length(q) - 1.) - th);

    float middle = box(p, vec2(w, th));

    return min(middle, min(circle1, circle2)) * s;
}

float sdfC(vec2 p) {
    float o = .72; // Circle extra width from the side
    p.x -= h;
    float circle = max(p.x - o, abs(length(p) - h + th) - th);
    return circle;
}

float sdfT(vec2 p) {
    float w = 1.5;
    return min(box(p, vec2(th, h)), box(p - vec2(0, h - th), vec2(w, th)));
}

float sdfI(vec2 p) {
    return box(p, vec2(th, h));
}

float sdfV(vec2 p) {
    float a = .4;
    p.x = abs(p.x);
    p.y += h - th - .2;
    p *= rot(a);
    return box(p, vec2(th, h * 2.));
}

float sdfPerspective(vec2 p) {
    // Bounds
    float d = sdfP(p + vec2(14, 0));
    d = min(d, sdfE(p + vec2(11, 0)));
    d = min(d, sdfR(p + vec2(8, 0)));
    d = min(d, sdfS(p + vec2(4.75, 0)));
    d = min(d, sdfP(p + vec2(3, 0)));
    d = min(d, sdfE(p + vec2(.5, 0)));
    d = min(d, sdfC(p - vec2(2, 0)));
    d = min(d, sdfT(p - vec2(7, 0)));
    d = min(d, sdfI(p - vec2(9.5, 0)));
    d = min(d, sdfV(p - vec2(12, 0)));
    d = min(d, sdfE(p - vec2(15, 0)));

    return d - 0.01;
}

int revisionData[] = int[10](0, -1, 0x7C0000, 0x3C0040F0, 0xFF01F7FD, -1, 0, 0xD39F0320, -1, 0x10001F);
float PI = 3.141592;

float sdRevisionLogo(in vec2 p) {
    float w = atan(p.y, p.x);
    float r = length(p);    
    // +32 because integers are rounded towards zero
    ivec2 i = ivec2(w * 16. / PI + 32., clamp(r * 10., 1., 8.)) - 1;
    float d = 1e6;
    for(int j = 0; j < 9; j++) {
        ivec2 k = i + ivec2(j % 3, j / 3);
        if((revisionData[k.y] >> k.x & 1) == 0)
            continue;
        float m = box(vec2(r * (mod(w / PI - float(k.x) / 16. + 31. / 32., 2.) - 1.) * PI, r - float(k.y) / 10. - .05), vec2(r * PI / 32., .05));
        d = min(d, m);
    }

    return d;
}

float sdfOctahedron(vec3 p, float s) {
    p = abs(p);
    float m = p.x + p.y + p.z - s;
    vec3 q;
    if(3.0 * p.x < m)
        q = p.xyz;
    else if(3.0 * p.y < m)
        q = p.yzx;
    else if(3.0 * p.z < m)
        q = p.zxy;
    else
        return m * 0.57735027;

    float k = clamp(0.5 * (q.z - q.y + s), 0.0, s);
    return length(vec3(q.x, q.y - s + k, q.z - k));
}

//MAP / SCENE FUNCTIONS
vec2 roomScene(vec3 p) {
    vec3 q = p - 4. * clamp(round(p / 4.), -vec3(1.), vec3(1.));
    vec2 spheres = vec2(length(q) - 1., 3.);

    vec2 room = vec2(-box(p, vec3(20.)), _s0);

    return minX(spheres, room);
}

vec2 pyramidScene(vec3 p) {
    p.xz -= round(p.xz / 13.) * 13.;
    return vec2(min(sdfOctahedron(p, 3.), p.y), 0.);
}

vec2 roadScene(vec3 p) {
    p.x = abs(p.x);
    p.x -= 5.;

    p.z -= round(p.z / 10.) * 10.;

    float cylinder = max(p.y - 5., length(p.xz) - 2.);

    return vec2(min(cylinder, p.y + 1.), 3);
}

vec2 mountainScene(vec3 p) {
    float dist = p.z - noise(p.xy) * .5 - 2.;
    return vec2(dist / 2., 3);
}

float sdLineSegment(vec3 p, vec3 a, vec3 b) {
    vec3 pa = p - a;
    vec3 ba = b - a;
    float h = clamp(dot(pa, ba) / dot(ba, ba), 0.0, 1.0);
    return length(pa - ba * h);
}

vec2 jungle(vec3 p) {
    p -= vec3(1, .3, 0);
    float S = 2.;
    vec3 id = round(p / S);
    float result = 1000.;

    vec3 origin = vec3(0);//hash33(id);

    p /= S;
    p -= id;

    for(int x = -1; x <= 1; x++) for(int y = -1; y <= 1; y++) for(int z = -1; z <= 1; z++) {
                vec3 q = vec3(x, y, z);

                vec3 location = q;//w + hash33(id + q);

                result = min(result, sdLineSegment(p, origin, location) - 0.011);
            }

    return vec2(result * S, 3);
}

float sdParabola(in vec2 pos, in float k) {
    pos.x = abs(pos.x);

    float ik = 1.0 / k;
    float p = ik * (pos.y - 0.5 * ik) / 3.0;
    float q = 0.25 * ik * ik * pos.x;

    float h = q * q - p * p * p;
    float r = sqrt(abs(h));

    float x = (h > 0.0) ? 
        // 1 root
    pow(q + r, 1.0 / 3.0) + pow(abs(q - r), 1.0 / 3.0) * sign(p) :
        // 3 roots
    2.0 * cos(atan(r, q) / 3.0) * sqrt(p);

    float d = length(pos - vec2(x, k * x * x));

    return (pos.x < x) ? -d : d;
}

vec2 parabola(vec3 p) {
    return vec2(-sdParabola(vec2(length(p.yx), p.z), .003), 2);
}

vec2 perspectiveScene(vec3 p) {
    return vec2(max(abs(p.z) - 0.03, -sdfPerspective(p.xy * 5.) / 5.), 2);
}

vec2 starScene(vec3 p) {
    vec3 rep = _s1 < 1. ? vec3(0) : _s1 < 2. ? vec3(1, 0, 0) : vec3(0, 1, 0);
    vec3 q = p - 6. * clamp(round(p / 6.), -rep, rep);
    q.xy *= rot(2.5);
    vec3 s = vec3(3, 1, 1);
    float result = sdfOctahedron(q / s, 1.);
    result = min(result, sdfOctahedron(q / s.yxy, 1.));
    result = min(result, sdfOctahedron(q / s.yyx, 1.));

    return vec2(result, 0);
}

vec2 revisionScene(vec3 p) {
    vec2 revisionLogo = vec2(max(abs(p.y) - .1, sdRevisionLogo(p.xz)), MAT_BLACK);
    vec2 disk = vec2(max(abs(p.y - .5) - .25, length(p.xz) - 1.2), MAT_WHITE);
    return minX(revisionLogo, disk);
}

vec2 map(vec3 p) {
    if(scene == 0) {
        return roomScene(p);
    } else if(scene == 1) {
        return perspectiveScene(p);
    } else if(scene == 2) {
        return pyramidScene(p);
    } else if(scene == 3) {
        return roadScene(p);
    } else if(scene == 4) {
        return jungle(p / 10.) * 10.;
    } else if(scene == 5) {
        return starScene(p);
    } else if(scene == 6) {
        // Fuck it, revision logo, I'm out of ideas
        return revisionScene(p);
    } else {
        // Empty scene
        return vec2(1000, 1);
    }
}

//RAY CAST / TRACE LOOP FUNCTION
vec2 raycast(vec3 rayOrigin, vec3 rayDirection) {
    vec2 dist, result = vec2(0.);                     //Distance & result variables
    for(int i = 0; i < 128; i++) {                        //Raymarching forward in loop
        dist = map(rayOrigin + rayDirection * result.x); //Get distance
        if(dist.x < near || result.x > far)
            break;      //.0001 = precision; If dist < precision OR gone too far: stop marching
        result.x += dist.x;
        result.y = dist.y;          //Jump forward and remember material we hit
    }
    return result;
}

#define FOV 1.000000

vec4 getDirectionProjection(vec3 direction) {
    vec3 forward = normalize(_pt - _pp);
    vec3 left = normalize(cross(forward, vec3(0, 1, 0)));
    vec3 up = normalize(cross(left, forward));

    float forwardFactor = dot(forward, direction) / FOV;

    vec2 uv = vec2(dot(direction, left), dot(direction, up)) / forwardFactor;

    // Fix aspect ratio
    uv.x *= _res.y / _res.x;
    uv += .5;

    // If forwardFactor is negative, the direction is behind the projector
    bool inBounds = clamp(uv.x, 0., 1.) == uv.x && clamp(uv.y, 0., 1.) == uv.y;
    return (forwardFactor > 0. && inBounds) ? texture(_tex0, uv) : vec4(0);
}

vec4 getSkyProjection(vec3 direction) {
    // March from the projector towards the sky direction
    vec2 marchResult = raycast(_pp, direction);

    // If it hit something, that something is gonna be what determines that pixel and not the sky
    return marchResult.x < far ? vec4(0) : getDirectionProjection(direction);
}

vec4 getProjection(vec3 position) {
    // March towards the projection location
    vec3 direction = normalize(position - _pp);
    float dist = length(position - _pp);

    vec2 marchResult = raycast(position - direction * 0.005, -direction);

    // If the march hit something, we're not in view of the "projector"
    return marchResult.x < dist ? vec4(0) : getDirectionProjection(direction);
}

vec3 skySynthwave(vec3 rd) {
    // return (scene == 0 ? vec3(.1, .2, .1) : scene == 1 ? vec3(.2, .1, .1) : vec3(.1, .1, .2)) - length(uv) * .1;
    vec2 uv = vec2(atan(rd.z, rd.x), rd.y);

    float factor = smoothstep(.3, 0., abs(cnoise(vec3(uv * 10., _t))));
    float factor1 = smoothstep(.9, 1., factor);
    float factor2 = smoothstep(0., .9, factor);

    return mix(mix(vec3(140, 30, 255) / 255., vec3(242, 34, 255) / 255., factor2), vec3(255, 211, 25) / 255., factor1);
}

vec3 skyNight(vec3 rd) {
    vec2 cloudUv = rd.xz / rd.y;
    float y = rd.y;
    vec2 uv = rd.xz / rd.y;

    vec3 topColor = vec3(0.012, 0.082, 0.184);
    vec3 bottomColor = vec3(0.306, 0.467, 0.604);
    vec3 skyColor = mix(bottomColor, topColor, pow(smoothstep(-.8, .2, y), .3));
    float clouds = fbm(cloudUv + _t * .1) * .105;
    float stars = smoothstep(0.055, .143, fbm(uv * 30.));
    skyColor += clouds * smoothstep(0., .3, rd.y) + stars * smoothstep(0., .3, rd.y);

    return skyColor;
}

vec3 skySunset(vec3 rd) {
    float stars = smoothstep(0.055, .143, fbm(rd.xz / rd.y * 30.));
    return mix(vec3(235, 91, 0) / 255., vec3(255, 178, 0) / 255., smoothstep(0., .4, rd.y)) + stars;
}

vec3 skyToWhite(vec3 rd) {
    return mix(vec3(235, 91, 0) / 255., vec3(1), smoothstep(-1., 1., rd.y));
}

vec3 sky(vec3 rd) {
    if(_s0 < 1.) {
        return skySynthwave(rd);
    } else if(_s0 < 2.) {
        return skyNight(rd);
    } else if(_s0 < 3.) {
        return skyToWhite(rd);
    } else if(_s0 < 4.) {
        return skySunset(normalize(rd));
    }
}

//float time; //If shader runs more than a few minutes, modulo _t by 10*PI to avoid noisy glitches due to sin precision, see below
void main() {
    vec2 fragCoord = gl_FragCoord.xy;
    //time=mod(_t,62.832); //If shader runs for more than few minutes, modulo _t by 20*pi and use "time" instead of "_t" everywhere
    vec2 uv = (fragCoord / (_res * _s3) - 0.5) / vec2(_res.y / _res.x, 1);         //Raymarching UVs
    //NOTE: IF you want to take HUGE screenshots then you MUST add "_shift" to fragCoord in the first pass
    //vec2 uv=((fragCoord+_shift)/_res-0.5)/vec2(_res.y/_res.x,1);
    vec3 rayOrigin = _cp;
    vec3 cameraForward = normalize(_ct - rayOrigin);
    vec3 cameraLeft = normalize(cross(cameraForward, vec3(0, 1, 0)));
    vec3 cameraUp = normalize(cross(cameraLeft, cameraForward));
    vec3 rayDirection = mat3(cameraLeft, cameraUp, cameraForward) * normalize(vec3(uv, FOV));
    vec3 lightDirection = normalize(vec3(.1, .4, -.3));
    vec3 lightDirection2 = normalize(vec3(-.1, .4, .3));
    vec3 backgroundColor = sky(rayDirection);          //Background: vec3(.1,.1,.1) = colour; -length(uv)*0.1 = cheap vignette
    vec4 skyProjection = getSkyProjection(rayDirection);
    vec3 color = backgroundColor;
    // Gamma correct the background but only the non-projected part
    color = pow(max(color, 0.), vec3(.4545));
    color = mix(color, skyProjection.rgb, skyProjection.a);
    vec2 result = raycast(rayOrigin, rayDirection);
    if(result.x < far) { // Not needed if you have fog, but could be more optimised, up for debate
        vec3 hitPos = rayOrigin + rayDirection * result.x;
        vec3 normals = normalize(ep.xyy * map(hitPos + ep.xyy).x + ep.yyx * map(hitPos + ep.yyx).x + ep.yxy * map(hitPos + ep.yxy).x +
            ep.xxx * map(hitPos + ep.xxx).x);

        // Override colors with projection
        vec4 projection = getProjection(hitPos);

        vec3 albedo = vec3(0.5, 0.5, 0.5);
        if(result.y >= MAT_BLACK)
            albedo = vec3(0);
        else if(result.y >= MAT_WHITE)
            albedo = vec3(1);
        else if(result.y >= MAT_RED)
            albedo = vec3(.8, .2, .2);
        else if(result.y >= MAT_YELLOW)
            albedo = vec3(.8, .8, .3);
        else if(result.y >= MAT_BLUE)
            albedo = vec3(.2, .2, .8);

        albedo = mix(albedo, projection.rgb, projection.a);

        float diffuse = min(1., max(0., dot(normals, lightDirection)) + max(0., dot(normals, lightDirection2)));
        float fresnel = min(1., pow(1. + dot(normals, rayDirection), 10.)); //Fresnel = background reflections on edges of geometry
        if(scene == 0 || scene == 2)
            fresnel = 0.; // Stop fresnel from ruining scenes where it doesn't look good
        if(scene == 0)
            diffuse = clamp(diffuse + dot(normals, -lightDirection), 0., 1.); // Light up the ceiling too
        float specular = pow(max(dot(reflect(-lightDirection, normals), -rayDirection), 0.), 30.);//Specular = Bright highlights; 30 = specular power
        float ao = clamp(map(hitPos + normals * .1).x / .1, 0., 1.);          //Ambient occlusion
        float sss = smoothstep(0., 1., map(hitPos + lightDirection * .4).x / .4);//SSS = Sub surface scattering / backlight; 0.4 = sss range
        color = result.y == MAT_WHITE ? albedo : mix(specular + albedo * (ao + .2) * (diffuse + sss * .5), backgroundColor, fresnel);//Final lighting made of all the above mixed with fresnel
        // co   lor=mix(backgroundColor,color,exp(-.000006*result.x*result.x*result.x));    //Add fog right at the end. 0.0001 = amount of fog

        // Gamma correct *before* applying projection
        // Avoid negative values: max(color,0.); Gamma correction: pow(x,vec3(.4545))
        color = pow(max(color, 0.), vec3(.4545));

        color = mix(color, projection.rgb, projection.a);

    }

    // Fade for last scene
    if(scene == 6)
        color *= _s1;

    fragColor = vec4(color, 1);
}