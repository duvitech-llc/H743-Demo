
#ifndef __MADGWICK_AHRS_H
#define __MADGWICK_AHRS_H

/* quaternion */
typedef union
{
   struct
   {
      float q0;
      float q1;
      float q2;
      float q3;
   };
   struct
   {
      float w;
      float x;
      float y;
      float z;
   };
   float vec[4];
}
quat_t;

typedef union
{
   struct
   {
      float x;
      float y;
      float z;
   };
   float vec[3];
}
vec3_t;


void madgwickSetGain(float gain);
void madgwickSetZeta(float zeta);

float madgwickGetGain(void);
float madgwickGetZeta(void);

void quat_mul(quat_t *o, const quat_t *q1, const quat_t *q2);
void quat_AngleAxis(quat_t *o, float angle,vec3_t *axis );

void madgwickAHRSupdate(
    vec3_t *a,
    vec3_t *g,
    vec3_t *m,
    float dt);


void getQVals(float * _q0, float* _q1, float* _q2, float* _q3);
void resetQVals(void);


#endif // __MADGWICK_AHRS_H
