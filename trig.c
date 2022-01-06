#include <stdio.h>
#include <stdlib.h>
#include <xc.h>

long sindegrees (int x){
    int sin;
//always wrap input angle to -PI..PI
if (x < -360){
    x += 720;}
else
if (x >  360){
    x -= 720;}

//compute sine
if (x < 0){
    sin = 7298 * x + 41 * x * x;}
else{
    sin = 7298 * x - 41 * x * x;
}
return sin;
}

long cosdegrees(int x){
    int cos;
//compute cosine: sin(x + PI/4) = cos(x)
x += 90;
if (x >  360){
    x -= 720;}

if (x < 0){
    cos = 7298 * x + 41 * x * x;}
else{
    cos = 7298 * x - 41* x * x;
}
return cos;

}
