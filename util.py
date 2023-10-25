import numpy as np

#read images from files
def imageRead(file: str,width:int,height:int,channels:int)-> np.array:
    num_bytes = width * height * channels
    # Read the raw image data
    with open(file, 'rb') as f:
        # Read the binary data into a NumPy array
        # 8-bit
        img_data = np.fromfile(f,dtype=np.uint8,count=num_bytes)
        # Reshape the array to match the image dimensions and channels
        img_data = img_data.reshape((height, width, channels))
    return img_data

#Subtract a number from pixel value
def PixelSubtract(image:np.array,value:int)->np.array:
    height, width, channels=np.shape(image)
    # print(height, width, channels)
    for i in range(height):
        for j in range(width):
            for h in range(channels):
                pixelvalue=image[i][j][h]
                pixelvalue=pixelvalue-value
                image[i][j][h]=pixelvalue
                # print(image[i][j][h])
    return image


def C(i:int):
    if i==0:
        return 1/np.sqrt(2)
    else:
        return 1
    
#Divid images into blocks
#data:array
#n:size of block
#BlockVec[[block1],[block2],...]
def blocking(data:np.array,n):
    print("blocking: shape of data",np.shape(data))
    height, width, channels=np.shape(data)
    BlockVec=[]
    for y in range(0, height, n):
        for x in range(0, width, n):
            for h in range(channels):
                block = data[y:y + n,x:x + n,h]
                # vector = block.flatten()
                BlockVec.append(block)
    return BlockVec

#DCT on a block
#image:N by N block
#return:N by N block
def DCT(image:np.array):
    height, width=np.shape(image)
    N=height
    Dctcoef=np.zeros((height, width),dtype=np.float32)
    # print(height, width, channels)
    for i in range(height):
        for j in range(width):
            Dctcoef[i][j]=(1/np.sqrt(2*N))*C(i)*C(j)*sum(image[a][b]*np.cos((2*a+1)*i*np.pi/(2*N))*np.cos((2*b+1)*j*np.pi/(2*N)) for b in range(width) for a in range(height))
            #print(Dctcoef[i][j])
    return Dctcoef           

def quantiMatrix(N:float):
    q50=np.array([[16,11,10,16,24,40,51,61],
         [12,12,14,19,26,58,60,55],
         [14,13,16,24,40,57,69,56],
         [14,17,22,29,51,87,80,62],
         [18,22,37,56,68,109,103,77],
         [24,35,55,64,81,104,113,92],
         [49,64,78,87,103,121,120,101],
         [72,92,95,98,112,100,103,99]])
    if(N>50):
        return ((100-N)/50)*q50
    else:
        return (50/N)*q50


def PSNR(data,quantized_data,bits):
    mse=np.mean((data-quantized_data)**2)
    return 10*np.log10((np.max(2**bits-1)**2)/mse)  

#gaussian filter    
def gaussian_filter(size:int,sigma:int):
    kernel=np.zeros((size,size))
    center=size//2
    for i in range(size):
        for j in range(size):
            x,y=i-center,j-center
            kernel[i][j]=(1/np.pi*sigma**2)*np.exp(-(x**2+y**2)/(2*sigma**2))
    return kernel/kernel.sum()

#please use odd-size kernel. e.g.5x5
#constant padding
def conv2d(image, kernel):
    # Dimensions of the input and kernel
    
    image_h, image_w = image.shape
    kernel_h, kernel_w = kernel.shape
    pad_h=kernel_h//2
    pad_w=kernel_w//2
    padded_image = np.pad(image, ((pad_h, pad_h), (pad_w, pad_w)), mode='constant')
    result=np.zeros((image_h,image_w))
    for i in range(image_h):
        for j in range(image_w):
            region=padded_image[i:i+kernel_h,j:j+kernel_w]
            result[i][j]=np.sum(region*kernel)
    return result

#image1_1: image array
#filter_size=N, N by N filter
#block_size:quantizetion blocks
#return: processed image
# def postprocessing(image1_1:np.array,filter_size:int,block_size:int):  
#     image_size=np.shape(image1_1)
#     print(image_size)
#     sigma=0.3*((filter_size-1)*0.5-1)+0.8
#     kernel=gaussian_filter(filter_size,sigma)
#     # print("before",image1_1)
#     result=np.copy(image1_1)
#     shift=int(block_size/2)-1
#     output=[]
#     for h in range(image_size[2]):
#         for i in range(0,image_size[0],block_size):
#             for j in range(0,image_size[0],block_size):
#                 if i+shift+block_size<image_size[0] and j+shift+block_size<image_size[0]:
#                     result[i+shift:i+shift+block_size,j+shift:j+shift+block_size][h]=conv2d(image1_1[i+shift:i+shift+block_size,j+shift:j+shift+block_size][h],kernel)
#                     if j==0 and i==0:
#                         print(result[i+shift:i+shift+block_size,j+shift:j+shift+block_size][h])
#                         print(np.shape(result[i+shift:i+shift+block_size,j+shift:j+shift+block_size][h]))
#                     #print(conv2d(image1_1[i+shift:i+shift+block_size,j+shift:j+shift+block_size][h],kernel))
#                     #output.append(conv2d(image1_1[i:i+block_size,j:j+block_size][h],kernel))
#                     #print(result[i:i+block_size,j:j+block_size][h],image1_1[i:i+block_size,j:j+block_size][h])
#     return result

def boundary_filter(img, block_size, filter_width,filter_size):
    output = img.copy()
    rows, cols = img.shape
    sigma=0.3*((filter_size-1)*0.5-1)+0.8
    kernel=gaussian_filter(filter_size,sigma)
    # Horizontal boundaries
    for i in range(block_size, rows, block_size):
        for j in range(0,cols-1,1):
            output[i-filter_width:i+filter_width, j:j+1] = conv2d(img[i-filter_width:i+filter_width, j:j+1],kernel)
            # np.mean(img[i-filter_width:i+filter_width, j])
    
    # Vertical boundaries
    for i in range(0,rows-1,1):
        for j in range(block_size, cols, block_size):
            output[i:i+1, j-filter_width:j+filter_width] = conv2d(img[i:i+1, j-filter_width:j+filter_width],kernel)
            #np.mean(img[i, j-filter_width:j+filter_width])
    
    return output

def shift_image(image, dx, dy):
    if dy > 0:
        image = np.vstack((image[-dy:], image[:-dy]))
    elif dy < 0:
        image = np.vstack((image[-dy:], image[:dy]))
    if dx > 0:
        image = np.hstack((image[:, -dx:], image[:, :-dx]))
    elif dx < 0:
        image = np.hstack((image[:, -dx:], image[:, :dx]))
    return image

import numpy as np

def shift_image_odd_symmetric(image, dx, dy):
    shifted = np.zeros_like(image)
    
    # Shift the image in the right and down direction
    if dx >= 0 and dy >= 0:
        shifted[dy:, dx:] = image[:-dy, :-dx]
        shifted[:dy, dx:] = -image[dy-1::-1, :-dx]
        shifted[dy:, :dx] = -image[:-dy, dx-1::-1]

    # Shift the image in the left and up direction
    elif dx < 0 and dy < 0:
        dx = abs(dx)
        dy = abs(dy)
        shifted[:-dy, :-dx] = image[dy:, dx:]
        shifted[-dy:, :-dx] = -image[:dy, dx:]
        shifted[:-dy, -dx:] = -image[dy:, :dx]

    # Shift the image in the right and up direction
    elif dx >= 0 and dy < 0:
        dy = abs(dy)
        shifted[:-dy, dx:] = image[dy:, :-dx]
        shifted[-dy:, dx:] = -image[:dy, :-dx]
        shifted[:-dy, :dx] = -image[dy:, dx-1::-1]

    # Shift the image in the left and down direction
    elif dx < 0 and dy >= 0:
        dx = abs(dx)
        shifted[dy:, :-dx] = image[:-dy, dx:]
        shifted[:dy, :-dx] = -image[dy-1::-1, dx:]
        shifted[dy:, -dx:] = -image[:-dy, :dx]
    
    return shifted


def shift_image_replication(image, dx, dy):
    # Create an empty output image
    shifted = np.zeros_like(image)
    
    # Shift the image in the right and down direction
    if dx >= 0 and dy >= 0:
        shifted[dy:, dx:] = image[:-dy, :-dx]
        shifted[:dy, dx:] = image[0, :-dx]
        shifted[dy:, :dx] = image[:-dy, 0:1]
        shifted[:dy, :dx] = image[0:1, 0:1]
    
    # Shift the image in the left and up direction
    elif dx < 0 and dy < 0:
        dx = abs(dx)
        dy = abs(dy)
        shifted[:-dy, :-dx] = image[dy:, dx:]
        shifted[-dy:, :-dx] = image[-1:, dx:]
        shifted[:-dy, -dx:] = image[dy:, -1:]
        shifted[-dy:, -dx:] = image[-1:, -1:]

    # Shift the image in the right and up direction
    elif dx >= 0 and dy < 0:
        dy = abs(dy)
        shifted[:-dy, dx:] = image[dy:, :-dx]
        shifted[:-dy, :dx] = image[dy:, 0:1]
        shifted[-dy:, dx:] = image[-1:, :-dx]
        shifted[-dy:, :dx] = image[-1:, 0:1]

    # Shift the image in the left and down direction
    elif dx < 0 and dy >= 0:
        dx = abs(dx)
        shifted[dy:, :-dx] = image[:-dy, dx:]
        shifted[:dy, :-dx] = image[0, dx:]
        shifted[dy:, -dx:] = image[:-dy, -1:]
        shifted[:dy, -dx:] = image[0:1, -1:]
    
    return shifted

def shift_image_zero_shift(image, dx, dy):
    # Create an empty output image filled with zeros
    shifted = np.zeros_like(image)
    
    # Shift the image in the right and down direction
    if dx >= 0 and dy >= 0:
        shifted[dy:, dx:] = image[:-dy, :-dx]
    
    # Shift the image in the left and up direction
    elif dx < 0 and dy < 0:
        dx = abs(dx)
        dy = abs(dy)
        shifted[:-dy, :-dx] = image[dy:, dx:]
        
    # Shift the image in the right and up direction
    elif dx >= 0 and dy < 0:
        dy = abs(dy)
        shifted[:-dy, dx:] = image[dy:, :-dx]

    # Shift the image in the left and down direction
    elif dx < 0 and dy >= 0:
        dx = abs(dx)
        shifted[dy:, :-dx] = image[:-dy, dx:]
    
    return shifted


    
# b=blocking(imageRead("../Original_Images/lena.raw",16,16,1),8)
# print(b)
# DCT(b[0],N=8)
# print(imageRead("../Original_Images/lena.raw",16,16,1))
#PixelSubtract(imageRead("../Original_Images/lena.raw",16,16,1),20)      
# kernel=gaussian_filter(7,1)
# q80=quantiMatrix(80)
# print(conv2d(q80,kernel))
# img=imageRead("../Original_Images/camera_man.raw",256,256,1)
# print(np.shape(postprocessing(img,5,8)[0]))
# print(np.shape(img))
# print(np.shape(boundary_filter(img[:,:,0],8,2,2)))
#print(postprocessing(img,5,8))

