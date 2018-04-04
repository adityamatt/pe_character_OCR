#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <string>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
using namespace std;
using namespace cv;


int* get_dimension(int x,int y,Mat input)
{
    int r=input.rows;
    int c=input.cols;
    int x1=x;
    int y1=y;
    int flagx=0;
    int flagy=0;
    for (int i=x;i<input.rows;i++)
    {
        for (int j=y;j<input.cols;j++)
        {
            if (flagx==0 && (int)input.at<uchar>(i,j)>0 && x1<i) 
            {
                x1=i;
                flagx=1;
            }
        }
    }
    //x1 is upper boundary right now
    for (int i=x;i<input.cols;i++)
    {
        for (int j=y;j<input.rows;j++)
        {
            if (flagy==0 && (int)input.at<uchar>(j,i)>0 && y1<i) 
            {
                y1=i;
                flagy=1;
            }
        }
    }
    //x1 is left boundary right now
    int x2=x1;
    int y2=y1;
    flagx=0;
    flagy=0;
    for (int i=x2;i<input.rows;i++)
    {
        flagx=1;
        flagy=0;
        for (int j=y2;j<input.cols;j++)
        {
            if ((int)input.at<uchar>(i,j)>0)
            {
                flagx=0;
                break;
            }
        }
        
        if (flagx==0) flagy=1;
        if (flagx==1 && flagy==1) break;
        for (int j=y2;j<input.cols;j++)
        {
            if (flagx==0 && (int)input.at<uchar>(i,j)>0 && x1<i) 
            {
                x1=i;
                flagx=1;
            }
        }
    }
    flagx=0;
    flagy=0;
    for (int i=y2;i<input.cols;i++)
    {
        flagy=1;
        flagx=0;
        for (int j=x2;j<input.cols;j++)
        {
            if ((int)input.at<uchar>(i,j)>0)
            {
                flagy=0;
                break;
            }
        }
        if (flagy==0) flagx=1;
        if (flagx==1 && flagy==1) break;
        for (int j=x2;j<input.rows;j++)
        {
            if (flagy==0 && (int)input.at<uchar>(j,i)>0 && y1<i) 
            {
                y1=i;
                flagy=1;
            }
        }
    }
    
    
    
    int* answer = new int[4];
    answer[0]=x1;                   //lower line
    answer[1]=x2;                   //upper line
    answer[2]=y1;                   //right line
    answer[3]=y2;                   //left line
    return answer;
}
Mat get_refined(Mat input,int* tmp1)
{
    Mat output(tmp1[0]-tmp1[1],tmp1[2]-tmp1[3],input.type());
    for (int i=tmp1[1];i<tmp1[0];i++)
    {
        for (int j=tmp1[3];j<tmp1[2];j++)
        {
            output.at<uchar>(i-tmp1[1],j-tmp1[3])=input.at<uchar>(i,j);
        }
    }
    return output;
}
void print_array(Mat input)
{
    for (int i=0;i<input.rows;i++)
    {
        for (int j=0;j<input.cols;j++)
        {
            int a = (int)input.at<uchar>(i,j);
            stringstream ss;
            ss << a;
            string output = ss.str();
            if (output.length()==1) 
            {
                cout<<output<<"   ";
            }
            else if (output.length()==2) 
            {
                cout<<output<<"  ";
            }
            else 
            {
                cout<<output<<" ";
            }
            
        }
        cout<<endl;
    }
}
void thresh(Mat input)
{
    for (int i=0;i<input.rows;i++)
    {
        for (int j=0;j<input.cols;j++)
        {
            if ((int)input.at<uchar>(i,j)<200) input.at<uchar>(i,j)=255;
            else input.at<uchar>(i,j)=0;
        }
    }
}
bool eight(int i,int j,Mat input)
{
    if ((i-1)>0 && (j-1)>0 && (int)input.at<uchar>(i-1,j-1)!=0) return true;
    else if ((i+1)<input.rows && (j-1)>0 && (int)input.at<uchar>(i+1,j-1)!=0) return true;
    else if ((i-1)>0 && (j+1)<input.cols && (int)input.at<uchar>(i-1,j+1)!=0) return true;
    else if ((i+1)<input.rows && (j+1)<input.cols && (int)input.at<uchar>(i+1,j+1)!=0) return true;
    else return false;
}
bool four(int i,int j,Mat input)
{
    if ((i-1)>0 && (int)input.at<uchar>(i-1,j)!=0) return true;
    else if ((j-1)>0 && (int)input.at<uchar>(i,j-1)!=0) return true;
    else if ((i+1)<input.rows && (int)input.at<uchar>(i+1,j)!=0) return true;
    else if ((j+1)<input.cols && (int)input.at<uchar>(i,j+1)!=0) return true;
    else return false;
}

Mat get_mask(Mat input)
{
    input=get_refined(input,get_dimension(0,0,input));
    //cout<<"Showing input to get_mask"<<endl;
    //imshow("Window1",input);
    //waitKey(0);
    Mat mask(input.rows,input.cols,input.type());
    //cout<<CV_32SC1<<" "<<input.type()<<endl;
    for (int i=0;i<input.rows;i++)
    {
        for (int j=0;j<input.cols;j++)
        {
            if ((int)input.at<uchar>(i,j)==255) mask.at<uchar>(i,j)=3;
            else if (four(i,j,input)==true) mask.at<uchar>(i,j)=2;
             else if (eight(i,j,input)==true) mask.at<uchar>(i,j)=1;
             else  mask.at<uchar>(i,j)=-1;
        }
    }
    return mask;
}

long function(Mat input,Mat mask)
{	//takes in a image, resizes to mask, then finds output
    //print_array(mask);
   Mat output=input;
    if (input.rows!=mask.rows || input.cols!=mask.cols)
    {
        float x=(float)mask.rows/input.rows;
        float y=(float)mask.cols/input.cols;
        //cout<<"Value of x and y "<<x<<" "<<y<<endl;
        resize(input,output,Size(y*input.cols,x*input.rows),0,0,INTER_NEAREST);
        //imshow("Resized_image",output);
        //waitKey(0);
    }
    long sum=0;
    for (int i=0;i<mask.rows;i++)
    {
        for (int j=0;j<mask.cols;j++)
        {
            if ((int)mask.at<uchar>(i,j)!=255)  
            	{
            		//cout<<sum<<" "<<(int)output.at<uchar>(i,j)<<" "<<(int)mask.at<uchar>(i,j)<<endl;
            		sum=sum+(int)output.at<uchar>(i,j)*(int)mask.at<uchar>(i,j);

            	}
            else 
            {
            	sum=sum-(int)input.at<uchar>(i,j);
            	//cout<<sum<<" "<<(int)input.at<uchar>(i,j)<<endl;
            }

        }
    }
    return sum;
}

float percentage(long x,long y)
{
    return ((float)x*100/y);
}

class Data
{
    public:
         char name;
         long function_output;
         Mat Mask;
         Data()
         {
            name=' ';
            function_output=0;
            Mask=Mat(0,0,0);
         }
         Data(char name1,Mat input)
         {
            name=name1;
            Mask=input;
            Mask=get_refined(Mask,get_dimension(0,0,Mask));
            Mask=get_mask(Mask);
            function_output=0;
         }
    
};
Data* create_data()
{
    Data* output = new Data[36];
    
    for (int i=0;i<10;i++)
    {
        string name="./Train/";
        string jpg=".jpg";
        stringstream convert;
        convert<<i;
        string result=convert.str();
        name=name+result+jpg;
        Mat tmp=imread(name,0);
        thresh(tmp);
        tmp=get_refined(tmp,get_dimension(0,0,tmp));
        Mat tmp_mask=get_mask(tmp);
        output[i].function_output=function(tmp,tmp_mask);
        output[i].Mask=tmp_mask;
        output[i].name=i+48;
    }
    for (int i=65;i<91;i++)
    {
    	string name="./Train/";
        string jpg=".jpg";
        stringstream convert;
        convert<<(char)i;
        string result=convert.str();
        name=name+result+jpg;
        Mat tmp=imread(name,0);
        thresh(tmp);
        tmp=get_refined(tmp,get_dimension(0,0,tmp));
        Mat tmp_mask=get_mask(tmp);
        output[i-55].function_output=function(tmp,tmp_mask);
        output[i-55].Mask=tmp_mask;
        output[i-55].name=i;
    }
    /*
    for (int i='a';i<='z';i++)
    {
      	//cout<<(char)i<<endl;
       	string name="./Train/";
	    string jpg=".jpg";
	    stringstream convert;
	    convert<<(char)i;
	    string result=convert.str();
	    name=name+result+jpg;
	    Mat tmp=imread(name,0);
       	thresh(tmp);
       	tmp=get_refined(tmp,get_dimension(0,0,tmp));
       	Mat tmp_mask=get_mask(tmp);
      	output[i-61].function_output=function(tmp,tmp_mask);
       	output[i-61].Mask=tmp_mask;
       	output[i-61].name=i;
    }
    */
    return output;
}
int absolute(int a)
{
    if (a>0) return a;
    else return -a;
}
char find(Data* data,Mat character,int n)
{
    
    
    character=get_refined(character,get_dimension(0,0,character));
    char x=' ';
    long min=2147483647;
    Mat output;

    for (int i=0;i<n;i++)
    {
        long y1=data[i].function_output;
        long x1=function(character,data[i].Mask);
        if (percentage(absolute(x1-y1),y1)<min)
        {
            x=data[i].name;
            min=percentage(absolute(x1-y1),y1);
        }
        
        cout<<" % "<<percentage(absolute(x1-y1),y1)<<" with compared character "<<data[i].name
        <<" and current x is "<<x<<" x1 y1: "<<x1<<" "<<y1<<endl;
    }
    return x;
}
int main(int argc,char** argv)
{
    string location;
    int del=0;
    
    while (argv[1][del]!=' ' && argv[1][del]!='\n' && argv[1][del]!='\0')
    {
        location.push_back(argv[1][del]);
        del++;
    }
    cout<<"Input image file is : "<<location<<endl;
    Data* data_set=create_data();
    for (int i=0;i<36;i++)
    {
        //cout<<data_set[i].function_output<<" "<<data_set[i].name<<endl;
    }
    //cout<<endl;
    Mat input=imread(location,0);
    thresh(input);
    Mat input1;
    input1=get_refined(input,get_dimension(0,0,input));
    Mat mask=get_mask(input1);
	//cout<<endl;
    //print_array(mask);
    //cout<<endl;
    cout<<"Identified character is : "<<find(data_set,input,36)<<endl;
    
}
