// build/bin/clang -cc1 -triple spirv-vulkan-compute -x hlsl -emit-llvm -finclude-default-header -disable-llvm-passes -o - RWBuffer.hlsl
// build/bin/clang -target spirv-vulkan-compute -x hlsl -hlsl-entry main -S -o - RWBuffer.hlsl

RWBuffer<float> rwb;
RWBuffer<float> rwb2;

[shader("compute")]
[numthreads(1,1,1)]
void main() {
  rwb[0] = 1.0;
  rwb2[0] = 2.0;
}
