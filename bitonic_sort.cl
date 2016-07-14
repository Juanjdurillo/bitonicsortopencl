// kernel.cl
// Perform bitonic sort
// Device code.


// OpenCL Kernel
__kernel void bitonic(__global float* A,  uint n, uint l)
{

	uint id = get_global_id(0);
	local float LA[16];
	
	LA[id] = A[id];
	LA[id + 8] = A[id + 8];
	barrier(CLK_LOCAL_MEM_FENCE);




	// PHASE 1. Making bitonic sequences
	for (uint i = 1; i <= l; i++) { 
	
		uint signo = (id >> (i - 1)) & 1;
		for (uint j = i; j>0; j--) { 
			
			uint t = 1 << (j-1);
			//uint g = 1 << j;


			uint index = (id >> (j-1));
			index = index << j;
			index = index + (id & (t - 1));
			uint index2 = index + t;

			if ((LA[index] > LA[index2]) ^ (signo)) {
				float aux = LA[index];
				LA[index] = LA[index2];
				LA[index2] = aux;
			}

			

			/*
			if ((LA[index] > LA[index2]) && (!signo)) { 
				float aux = LA[index];
				LA[index] = LA[index2];
				LA[index2] = aux;
			} 
			
			if ((LA[index] < LA[index2]) && (signo)) { 
				float aux	= LA[index];
				LA[index]	= LA[index2];
				LA[index2]	= aux;
			}*/

//			printf("(%d -  %d , %d) %d %d\n", id, t, signo, index, index2);

			barrier(CLK_LOCAL_MEM_FENCE);
		}
	}

	/*
	for (uint j = l; j > 0; j--) {

		uint t = 1 << (j - 1);
		uint g = 1 << j;


		uint index = (id >> (j - 1));
		index = index << j;
		index = index + (id & (t - 1));
		uint index2 = index + t;


		if ((LA[index] > LA[index2])) {
			float aux = LA[index];
			LA[index] = LA[index2];
			LA[index2] = aux;
		}

	//	printf("(%d -  %d) %d %d\n", id, t, index, index2);

		barrier(CLK_LOCAL_MEM_FENCE);

	}
	*/
	A[id] = LA[id];
	A[id + 8] = LA[id + 8];


}
