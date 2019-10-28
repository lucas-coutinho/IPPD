__kernel void mult(__global int *a,
                  __global int *b,
                  __global int *out)
{
    int num = get_global_id(0);
    out[num] = a[num]*b[num];
    
}
