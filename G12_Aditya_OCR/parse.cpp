#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <string>
#include <iostream>
using namespace std;
using namespace cv;
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
    //y1 is left boundary right now
    int x2=x1;
    int y2=y1;
    flagx=0;
    flagy=0;
    for (int i=x2;i<input.rows;i++)
    {
        flagx=1;
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
        for (int j=x2;j<input.cols;j++)
        {
            if ((int)input.at<uchar>(i,j)>0)
            {
                flagy=0;
                break;
            }
        }
        if (flagy==0) flagx=1;
        if (flagx==1 && flagy==1) 
            {   

                break;
            }
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
class char_list
{
    private:
        int start_x;
        int start_y;
        
    public:
        bool has_next;
        Mat default_image;
        Mat next_char;
        char_list(string input)
        {
            has_next=true;
            start_x=0;
            start_y=0;
            default_image=imread(input,0);
            thresh(default_image);
            imshow("Window1",default_image);
            waitKey(0);
        }
        Mat get_nextChar()
        {
            //cout<<"Next char started"<<endl;
            Mat nothing=Mat(0,0,default_image.type());
            if ((start_x>=default_image.cols || start_y>=default_image.rows || has_next==false)) 
            {   
                //cout<<"Hello"<<endl;
                has_next=false;
                return nothing;
                
            }

            cout<<"start_x : "<<start_x<<" start_y: "<<start_y<<endl;
            int* tmp1=get_dimension(start_y,start_x,default_image);
            //cout<<tmp1[0]<<" "<<tmp1[1]<<" "<<tmp1[2]<<" "<<tmp1[3]<<" "<<endl;
            Mat trunk1=get_refined(default_image,tmp1);
            int* tmp2=get_dimension(0,0,trunk1);
            Mat trunk2=get_refined(trunk1,tmp2);
            
            if (start_x+tmp1[2]<=default_image.rows) start_x=start_x+tmp1[2];
            else
            {
                if (start_y+tmp1[0]<=default_image.cols) 
                {
                    start_x=0;
                    start_y=start_y+tmp1[0];
                }
                else
                {
                    has_next=false;
                    Mat return_null(0,0,default_image.type());
                    return return_null;
                }
            }
            if (trunk2.rows!=0 && trunk2.cols!=0)
            {
                imshow("Window1",trunk2);
                waitKey(0);
            }
            //cout<<"Function Terminated"<<endl;
            return trunk1;
        }
    
};
int main(int argc,char** argv)
{    namedWindow("Window1",CV_WINDOW_AUTOSIZE);
    string location;
    int del=0;
    while (argv[1][del]!=' ' && argv[1][del]!='\n' && argv[1][del]!='\0')
    {
        location.push_back(argv[1][del]);
        del++;
    }
    cout<<location<<endl;
    char_list object=char_list(location);
    int n=0;
    while (object.has_next==true)
    {
        object.get_nextChar();
    }
    
    Mat input=imread(location,0);
    thresh(input);
    //print_array(input);
    
    int* tmp1=get_dimension(0,0,input);
    cout<<tmp1[0]<<" "<<tmp1[1]<<" "<<tmp1[2]<<" "<<tmp1[3]<<" "<<endl;
    
    /*
    for (int i=0;i<input.cols;i++)
    {
        input.at<uchar>(tmp1[0],i)=255;
    }
    for (int i=0;i<input.cols;i++)
    {
        input.at<uchar>(tmp1[1],i)=255;
    }
    for (int i=0;i<input.rows;i++)
    {
        input.at<uchar>(i,tmp1[2])=255;
    }
    for (int i=0;i<input.rows;i++)
    {
        input.at<uchar>(i,tmp1[3])=255;
    }
    */
    Mat trunk1=get_refined(input,tmp1);
    //cout<<"Trunk1 cols and rows are : "<<trunk1.cols<<" "<<trunk1.rows<<endl;
    int* tmp2=get_dimension(0,0,trunk1);
    cout<<tmp2[0]<<" "<<tmp2[1]<<" "<<tmp2[2]<<" "<<tmp2[3]<<" "<<endl;
    Mat trunk2=get_refined(trunk1,tmp2);
    cout<<"Trunk2 cols and rows are : "<<trunk2.cols<<" "<<trunk2.rows<<endl;
    //imshow("Window1",input);
    //cout<<get_value(trunk2);
    //waitKey(0);
}
