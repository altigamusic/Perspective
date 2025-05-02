param (
[String] $ShaderPath = "src\FragmentShader.glsl"
)

if (!(Test-Path $ShaderPath)) {
    Write-Host "The fragment shader doesn't exist, skipping."
    exit 0
}

# Read the shader
$shaderData = Get-Content -Raw $ShaderPath

# There's an "#ifdef ACTUAL_CODE" and "#else" section in the start of the file - I want to keep only
# what's inside the ACTUAL_CODE segment, and remove everything else before the minifier.
# This is because that stuff is only used for testing inside poshbrolly.
$shaderData = $shaderData -replace "(?ms)#ifdef ACTUAL_CODE(.*?)#else.*?#endif", "`$1"

# We now add some more boilerplate for conversion between poshbrolly and actual code
$shaderData = "#version 330
const vec2 _res = vec2(1920,1080);
$shaderData"

# Now we save the result to a temp file and minify it
$tempFileName = "temp-shader-$((New-Guid).Guid).glsl"

Write-Output "$shaderData" > $tempFileName

& .\shader_minifier.exe --aggressive-inlining -v -o src\shader.inl $tempFileName

Remove-Item $tempFileName

# The shader variable is named after the file (and you can't change it...) so we just regex-replace it
# (the . becomes _ in the result, but . captures any char in regex, so we don't have to do anything else)
# Also we can't pipe the result to stdout, because the verbose log is ALSO piped to stdout (WHY NOT STDERR)
(Get-Content src\shader.inl) -replace "false","1<0" -replace "true","1>0" -replace "$tempFilename", "fragmentShaderSource" | Out-File src\shader.inl
