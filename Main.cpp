#include <iostream>
#include <cstdlib>
#include <iomanip>

//#ifndef BTC_H
//#define BTC_H

#include "BTC.h"

//#endif // BTC_H

#include "MyAlgorithm.h"
#include "BmCombineQuan.h"
#include "QuanCombineBm.h"
#include "BmCombineQuan_Sec.h"
//#include "Block.h"

int main(void){
    std::string file;
    std::cin >> file;
    BTC* btc = new BTC();
    //BmCombineQuan* myMethod = new BmCombineQuan();
    //BmCombineQuan_Sec* myMethod = new BmCombineQuan_Sec();
    QuanCombineBm* myMethod = new QuanCombineBm(4,1001);

    MyAlgorithm* myAlgorithm = new MyAlgorithm();

    btc->BTC_Compress((char *)file.c_str());

    myAlgorithm->SetBlockData(btc->GetBlockData());
    myAlgorithm->SetMethod(myMethod);

    std::cout << "Embed or Detected: ";
    std::string EorD;

    std::cin >> EorD;

    //btc->Show_block(127,122);

    if(EorD == "E" || EorD == "e"){
        myAlgorithm->StartEmbed();

        std:: cout << std::endl;
        //myMethod->Attack(btc->GetBlockData());
        std::cout << "Attack?: ";

        std::string YorN;
        std::cin >> YorN;

        btc->Decompress();

        std::string newfile = "New_" + file;
        btc->Ouput((char *)newfile.c_str());

        //btc->Show_block(127,122);

        std::cout << btc->PSNR() << std::endl;

        BTC* attack_btc = new BTC();
        std::string attackFile = "123.raw";
        attack_btc->BTC_Compress((char*)attackFile.c_str());

        myAlgorithm->AttackImage(btc->GetChangeImage(),attack_btc->GetOriginalImage());

        std::string beAttackFile = "Attack_New_" + file;
        btc->Ouput((char *)beAttackFile.c_str());

        std::cout << btc->PSNR() << std::endl;

        delete attack_btc;

        /*if(YorN == "Y" || YorN == "y"){
            BTC* attack_btc = new BTC();
            std::string attackFile = "123.raw";
            attack_btc->BTC_Compress((char*)attackFile.c_str());

            myAlgorithm->AttackImage(btc->GetChangeImage(),attack_btc->GetOriginalImage());

            file = "Attack_New_" + file;
            btc->Ouput((char *)file.c_str());

            delete attack_btc;
        }
        else{
            file = "New_" + file;
            btc->Ouput((char *)file.c_str());
        }*/

        //btc->Show_block(0,7);

        //std::cout << btc->PSNR() << std::endl;
    }

    else if(EorD == "D" || EorD == "d"){
        myAlgorithm->StartDetected();
        //myAlgorithm->RefinementDetected();

        std::string comparisonImage;
        std::cout << "Input Comparison Image:";
        std::cin >> comparisonImage;

        /**/btc->Show_block(32,126);

        BTC* compareBTC = new BTC();
        compareBTC->BTC_Compress((char*)comparisonImage.c_str());

        /**/compareBTC->Show_block(32,126);

        int false_positive = 0;
        int false_negative = 0;
        int true_negative = 0;
        int true_positive = 0;

        for(int h = 0; h < H/b_h; h ++){
            for(int w = 0; w < W/b_w; w++){
                bool dirtyFlag = false;
                if(compareBTC->GetBlockData()[h][w].q_a != btc->GetBlockData()[h][w].q_a){
                    dirtyFlag = true;
                }
                else if(compareBTC->GetBlockData()[h][w].q_b != btc->GetBlockData()[h][w].q_b){
                    dirtyFlag = true;
                }
                else{
                    Bitmap* bitmap = btc->GetBlockData()[h][w].bitmap;
                    Bitmap* compare_bitmap = compareBTC->GetBlockData()[h][w].bitmap;

                    for(int sub_h = 0; sub_h < b_h; sub_h++){
                        for(int sub_w = 0; sub_w < b_w; sub_w++){
                            if(bitmap->bm[sub_h][sub_w] != compare_bitmap->bm[sub_h][sub_w]){
                                dirtyFlag = true;
                                break;
                            }
                        }
                    }

                    bitmap = NULL;
                    compare_bitmap = NULL;
                }

                if(dirtyFlag != btc->GetBlockData()[h][w].dirty){
                    if(btc->GetBlockData()[h][w].dirty == true){
                        false_positive++;
                    }
                    else{
                        false_negative++;
                    }
                }
                else{
                    if(btc->GetBlockData()[h][w].dirty == true){
                        true_positive++;
                    }
                    else{
                        true_negative++;
                    }
                }
            }
        }

        int falseCount = false_positive + false_negative;
        int totalDirtyCount = myAlgorithm->GetTotalDirtyCount();
        double correctRate = (1.0 - ((double)falseCount / (double)totalDirtyCount)) * 100.0;

        std::cout << "True Positive : " << true_positive  << std::endl;
        std::cout << "False Negative : " << false_negative << std::endl;
        std::cout << "True Negative : " << true_negative << std::endl;
        std::cout << "False Positive : " << false_positive << std::endl;
        std::cout << "Wrong Detected : " << falseCount << std::endl;
        std::cout << "detected blocks : " << totalDirtyCount << std::endl;
        std::cout << "The correct rate is : " << std::fixed << std::setprecision(4) << correctRate << std::endl;

        std::string detectedFile = "Detected_" + file;
        myAlgorithm->OutPutDetected((char*)detectedFile.c_str());

        delete compareBTC;
    }
    else{
        int a = 3;
        int b = a / 2;
        std::cout << b << std::endl;
    }


    delete myAlgorithm;
    delete btc;

    return 0;
}
