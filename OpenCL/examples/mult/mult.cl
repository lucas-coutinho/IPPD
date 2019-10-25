_kernel void mult(_global int *a,
                  _global int *b,
                  _global int *out)
{
    int num = get_global_id(0);
    printf("%d \n", num);
	out[num] = a[num]*b[num];
    
}