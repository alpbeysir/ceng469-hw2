# Computer Graphics II - Median Cut Environment Lighting

This assignment involved the application of advanced HDR environment lighting with OpenGL. Different from the previous homework, I made the decision to use C# instead C++ because I wanted to see how my favourite language would
handle the OpenGL API. I used the starter files from the open-source Silk.NET bindings, which provide functionality similar to the sampleGL files provided with the homework.

## Equilateral HDR image to cubemap texture

The first step is getting our .hdr to the correct format. We have one image that represents the entire sky, but we need 6 separate images for each direction. There are multiple methods of achieving this conversion. I went with the method
where one renders a fullscreen quad 6 times for each direction. A special fragment shader is used to sample the .hdr. This shader calculates what color each pixel in each face should be by looking at the input HDRI. After this processing
is complete, we have a cubemap texture ready for environment rendering.
