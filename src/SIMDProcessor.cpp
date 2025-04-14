#include "SIMDProcessor.hpp"

PF_Err SIMDProcessor::ProcessPixelBlock8(
    const PF_Pixel8* inP,
    PF_Pixel8* outP,
    const CurveData* curves,
    A_long numPixels)
{
    PF_Err err = PF_Err_NONE;

    // Process 8 pixels at a time using AVX2
    for (A_long i = 0; i < numPixels; i += SIMD_BLOCK_SIZE) {
        // Load 8 pixels
        __m256i pixels = _mm256_loadu_si256((__m256i*)&inP[i]);

        // Extract channels
        __m256 red = _mm256_cvtepi32_ps(_mm256_and_si256(pixels, _mm256_set1_epi32(0xFF)));
        __m256 green = _mm256_cvtepi32_ps(_mm256_srli_epi32(pixels, 8));
        __m256 blue = _mm256_cvtepi32_ps(_mm256_srli_epi32(pixels, 16));

        // Normalize to [0,1]
        red = _mm256_div_ps(red, _mm256_set1_ps(255.0f));
        green = _mm256_div_ps(green, _mm256_set1_ps(255.0f));
        blue = _mm256_div_ps(blue, _mm256_set1_ps(255.0f));

        // Apply curves
        red = EvaluateCurveSIMD(&curves[1], red);    // Red curve
        green = EvaluateCurveSIMD(&curves[2], green); // Green curve
        blue = EvaluateCurveSIMD(&curves[3], blue);   // Blue curve

        // Convert back to 8-bit
        red = _mm256_mul_ps(red, _mm256_set1_ps(255.0f));
        green = _mm256_mul_ps(green, _mm256_set1_ps(255.0f));
        blue = _mm256_mul_ps(blue, _mm256_set1_ps(255.0f));

        // Pack and store
        __m256i result = _mm256_or_si256(
            _mm256_cvtps_epi32(red),
            _mm256_slli_epi32(_mm256_cvtps_epi32(green), 8)
        );
        result = _mm256_or_si256(
            result,
            _mm256_slli_epi32(_mm256_cvtps_epi32(blue), 16)
        );

        _mm256_storeu_si256((__m256i*)&outP[i], result);
    }

    return err;
}

PF_Err SIMDProcessor::ProcessPixelBlockFloat(
    const PF_PixelFloat* inP,
    PF_PixelFloat* outP,
    const CurveData* curves,
    A_long numPixels)
{
    PF_Err err = PF_Err_NONE;

    // Process 8 pixels at a time using AVX
    for (A_long i = 0; i < numPixels; i += SIMD_BLOCK_SIZE) {
        // Load 8 pixels worth of each channel
        __m256 red = _mm256_loadu_ps(&inP[i].red);
        __m256 green = _mm256_loadu_ps(&inP[i].green);
        __m256 blue = _mm256_loadu_ps(&inP[i].blue);

        // Apply master RGB curve
        __m256 rgb_r = EvaluateCurveSIMD(&curves[0], red);
        __m256 rgb_g = EvaluateCurveSIMD(&curves[0], green);
        __m256 rgb_b = EvaluateCurveSIMD(&curves[0], blue);

        // Apply individual channel curves
        red = EvaluateCurveSIMD(&curves[1], rgb_r);
        green = EvaluateCurveSIMD(&curves[2], rgb_g);
        blue = EvaluateCurveSIMD(&curves[3], rgb_b);

        // Store results
        _mm256_storeu_ps(&outP[i].red, red);
        _mm256_storeu_ps(&outP[i].green, green);
        _mm256_storeu_ps(&outP[i].blue, blue);
    }

    return err;
}

__m256 SIMDProcessor::EvaluateCurveSIMD(
    const CurveData* curve,
    __m256 x)
{
    __m256 result = _mm256_setzero_ps();

    // Find segment containing x for each lane
    for (A_long i = 0; i < curve->num_points - 1; i++) {
        __m256 x0 = _mm256_set1_ps(curve->points[i].x);
        __m256 x1 = _mm256_set1_ps(curve->points[i + 1].x);
        __m256 y0 = _mm256_set1_ps(curve->points[i].y);
        __m256 y1 = _mm256_set1_ps(curve->points[i + 1].y);

        // Calculate mask for points in this segment
        __m256 mask = _mm256_and_ps(
            _mm256_cmp_ps(x, x0, _CMP_GE_OQ),
            _mm256_cmp_ps(x, x1, _CMP_LE_OQ)
        );

        // Interpolate for points in this segment
        __m256 t = _mm256_div_ps(_mm256_sub_ps(x, x0), _mm256_sub_ps(x1, x0));
        __m256 y = _mm256_add_ps(y0, _mm256_mul_ps(t, _mm256_sub_ps(y1, y0)));

        // Blend with existing result based on mask
        result = _mm256_blendv_ps(result, y, mask);
    }

    return result;
}