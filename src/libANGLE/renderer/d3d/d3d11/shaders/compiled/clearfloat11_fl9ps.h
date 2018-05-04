#if 0
//
// Generated by Microsoft (R) HLSL Shader Compiler 6.3.9600.16384
//
//
// Buffer Definitions: 
//
// cbuffer ColorAndDepthDataFloat
// {
//
//   float4 color_Float;                // Offset:    0 Size:    16
//   float zValueF_Float;               // Offset:   16 Size:     4
//
// }
//
//
// Resource Bindings:
//
// Name                                 Type  Format         Dim Slot Elements
// ------------------------------ ---------- ------- ----------- ---- --------
// ColorAndDepthDataFloat            cbuffer      NA          NA    0        1
//
//
//
// Input signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// SV_POSITION              0   xyzw        0      POS   float       
//
//
// Output signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// SV_TARGET                0   xyzw        0   TARGET   float   xyzw
// SV_TARGET                1   xyzw        1   TARGET   float   xyzw
// SV_TARGET                2   xyzw        2   TARGET   float   xyzw
// SV_TARGET                3   xyzw        3   TARGET   float   xyzw
// SV_DEPTH                 0    N/A   oDepth    DEPTH   float    YES
//
//
// Constant buffer to DX9 shader constant mappings:
//
// Target Reg Buffer  Start Reg # of Regs        Data Conversion
// ---------- ------- --------- --------- ----------------------
// c0         cb0             0         2  ( FLT, FLT, FLT, FLT)
//
//
// Level9 shader bytecode:
//
    ps_2_x
    mov oC0, c0
    mov oC1, c0
    mov oC2, c0
    mov oC3, c0
    mov oDepth, c1.x

// approximately 5 instruction slots used
ps_4_0
dcl_constantbuffer cb0[2], immediateIndexed
dcl_output o0.xyzw
dcl_output o1.xyzw
dcl_output o2.xyzw
dcl_output o3.xyzw
dcl_output oDepth
mov o0.xyzw, cb0[0].xyzw
mov o1.xyzw, cb0[0].xyzw
mov o2.xyzw, cb0[0].xyzw
mov o3.xyzw, cb0[0].xyzw
mov oDepth, cb0[1].x
ret 
// Approximately 6 instruction slots used
#endif

const BYTE g_PS_ClearFloat_FL9[] =
{
     68,  88,  66,  67,  50, 112, 
    200, 244,  43, 179,  42,  60, 
      1,  54, 148, 142, 159,  31, 
    160, 168,   1,   0,   0,   0, 
    228,   3,   0,   0,   6,   0, 
      0,   0,  56,   0,   0,   0, 
    180,   0,   0,   0, 132,   1, 
      0,   0,   0,   2,   0,   0, 
     20,   3,   0,   0,  72,   3, 
      0,   0,  65, 111, 110,  57, 
    116,   0,   0,   0, 116,   0, 
      0,   0,   0,   2, 255, 255, 
     68,   0,   0,   0,  48,   0, 
      0,   0,   1,   0,  36,   0, 
      0,   0,  48,   0,   0,   0, 
     48,   0,   0,   0,  36,   0, 
      0,   0,  48,   0,   0,   0, 
      0,   0,   2,   0,   0,   0, 
      0,   0,   0,   0,   1,   2, 
    255, 255,   1,   0,   0,   2, 
      0,   8,  15, 128,   0,   0, 
    228, 160,   1,   0,   0,   2, 
      1,   8,  15, 128,   0,   0, 
    228, 160,   1,   0,   0,   2, 
      2,   8,  15, 128,   0,   0, 
    228, 160,   1,   0,   0,   2, 
      3,   8,  15, 128,   0,   0, 
    228, 160,   1,   0,   0,   2, 
      0,   8,  15, 144,   1,   0, 
      0, 160, 255, 255,   0,   0, 
     83,  72,  68,  82, 200,   0, 
      0,   0,  64,   0,   0,   0, 
     50,   0,   0,   0,  89,   0, 
      0,   4,  70, 142,  32,   0, 
      0,   0,   0,   0,   2,   0, 
      0,   0, 101,   0,   0,   3, 
    242,  32,  16,   0,   0,   0, 
      0,   0, 101,   0,   0,   3, 
    242,  32,  16,   0,   1,   0, 
      0,   0, 101,   0,   0,   3, 
    242,  32,  16,   0,   2,   0, 
      0,   0, 101,   0,   0,   3, 
    242,  32,  16,   0,   3,   0, 
      0,   0, 101,   0,   0,   2, 
      1, 192,   0,   0,  54,   0, 
      0,   6, 242,  32,  16,   0, 
      0,   0,   0,   0,  70, 142, 
     32,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  54,   0, 
      0,   6, 242,  32,  16,   0, 
      1,   0,   0,   0,  70, 142, 
     32,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  54,   0, 
      0,   6, 242,  32,  16,   0, 
      2,   0,   0,   0,  70, 142, 
     32,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  54,   0, 
      0,   6, 242,  32,  16,   0, 
      3,   0,   0,   0,  70, 142, 
     32,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  54,   0, 
      0,   5,   1, 192,   0,   0, 
     10, 128,  32,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
     62,   0,   0,   1,  83,  84, 
     65,  84, 116,   0,   0,   0, 
      6,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      5,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   5,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  82,  68,  69,  70, 
     12,   1,   0,   0,   1,   0, 
      0,   0,  84,   0,   0,   0, 
      1,   0,   0,   0,  28,   0, 
      0,   0,   0,   4, 255, 255, 
      0,   1,   0,   0, 216,   0, 
      0,   0,  60,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      1,   0,   0,   0,  67, 111, 
    108, 111, 114,  65, 110, 100, 
     68, 101, 112, 116, 104,  68, 
     97, 116,  97,  70, 108, 111, 
     97, 116,   0, 171,  60,   0, 
      0,   0,   2,   0,   0,   0, 
    108,   0,   0,   0,  32,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0, 156,   0, 
      0,   0,   0,   0,   0,   0, 
     16,   0,   0,   0,   2,   0, 
      0,   0, 168,   0,   0,   0, 
      0,   0,   0,   0, 184,   0, 
      0,   0,  16,   0,   0,   0, 
      4,   0,   0,   0,   2,   0, 
      0,   0, 200,   0,   0,   0, 
      0,   0,   0,   0,  99, 111, 
    108, 111, 114,  95,  70, 108, 
    111,  97, 116,   0,   1,   0, 
      3,   0,   1,   0,   4,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 122,  86,  97, 108, 
    117, 101,  70,  95,  70, 108, 
    111,  97, 116,   0, 171, 171, 
      0,   0,   3,   0,   1,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  77, 105, 
     99, 114, 111, 115, 111, 102, 
    116,  32,  40,  82,  41,  32, 
     72,  76,  83,  76,  32,  83, 
    104,  97, 100, 101, 114,  32, 
     67, 111, 109, 112, 105, 108, 
    101, 114,  32,  54,  46,  51, 
     46,  57,  54,  48,  48,  46, 
     49,  54,  51,  56,  52,   0, 
    171, 171,  73,  83,  71,  78, 
     44,   0,   0,   0,   1,   0, 
      0,   0,   8,   0,   0,   0, 
     32,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   0,  15,   0,   0,   0, 
     83,  86,  95,  80,  79,  83, 
     73,  84,  73,  79,  78,   0, 
     79,  83,  71,  78, 148,   0, 
      0,   0,   5,   0,   0,   0, 
      8,   0,   0,   0, 128,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
     15,   0,   0,   0, 128,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   1,   0,   0,   0, 
     15,   0,   0,   0, 128,   0, 
      0,   0,   2,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   2,   0,   0,   0, 
     15,   0,   0,   0, 128,   0, 
      0,   0,   3,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   3,   0,   0,   0, 
     15,   0,   0,   0, 138,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0, 255, 255, 255, 255, 
      1,  14,   0,   0,  83,  86, 
     95,  84,  65,  82,  71,  69, 
     84,   0,  83,  86,  95,  68, 
     69,  80,  84,  72,   0, 171
};
