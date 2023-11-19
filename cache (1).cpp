#include "cache.h"


cache::cache()
{
	for (int i=0; i<L1_CACHE_SETS; i++)
		L1[i].valid = false;
	for (int i=0; i<L2_CACHE_SETS; i++)
		for (int j=0; j<L2_CACHE_WAYS; j++){
			L2[i][j].valid = false;
			L2[i][j].lru_position=-1;
		}


	this->myStat.missL1 =0;
	this->myStat.missL2 =0;
	this->myStat.accL1 =0;
	this->myStat.accL2 =0;
}
//update because of evicting
void cache::updateLRU(int index,int newTag,int newData)
{




}


bool cache::searchL1(string real_adr){
	string str_index=real_adr.substr(28,4);
	string str_tag=real_adr.substr(0,28);
	int adr_index= binary_string_to_decimal(str_index);
	int adr_tag = binary_string_to_decimal(str_tag);
	return L1[adr_index].valid&&(L1[adr_index].tag==adr_tag);
}

int cache::searchL2(string real_adr) {
	string str_index=real_adr.substr(28,4);
	string str_tag=real_adr.substr(0,28);
	int adr_index= binary_string_to_decimal(str_index);
	int adr_tag = binary_string_to_decimal(str_tag);
	for (int i = 0; i < 8; i++) {
		if (L2[adr_index][i].valid && (L2[adr_index][i].tag == adr_tag)) {
			return i;
		}
	}
	return -1;
}



void cache::writeToL1(int index,int newTag,int newData,string real_adr){
	if(L1[index].valid==false){
		L1[index].data = newData ;
		L1[index].tag = newTag;
		L1[index].valid=true;

	}
	else{
		int temp_data=L1[index].data;
		int temp_tag=L1[index].tag;
		writeToL2(index,temp_tag,temp_data,real_adr);
		//evict
		L1[index].data=newData;
		L1[index].tag=newTag;

	}


}
int cache::findway(int index){
	for(int i=0;i<8;i++){
		if(L2[index][i].lru_position==-1){

			return i;
		}
		if(L2[index][i].lru_position==0){
			return i;
		}

	}

	return 0;
}

void cache::writeToL2(int index,int newTag,int newData,string real_adr) {
	int way=findway(index);

	L2[index][way].tag=newTag;
	L2[index][way].data=newData;
	L2[index][way].valid=true;
	for(int i=0;i<8;i++){
		if((L2[index][i].lru_position!=-1)&&(L2[index][i].lru_position!=0)){
			L2[index][i].lru_position--;

		}
	}
	L2[index][way].lru_position=7;

}





bool cache::controller(bool MemR, bool MemW, int* data, int adr, int* myMem)
{

	string real_adr = bitset<32>(adr).to_string();
	string str_index=real_adr.substr(28,4);
	string str_tag=real_adr.substr(0,28);
	int adr_index= binary_string_to_decimal(str_index);
	int adr_tag = binary_string_to_decimal(str_tag);
	int L2_token = searchL2(real_adr);
	if(MemW==true){
		myMem[adr] = *data;
		if(searchL1(real_adr)){
			this->myStat.accL1++;



			//writeToL1(adr_index,adr_tag,myMem[adr]);

		}
		//if(searchL1(real_adr)==false)
		else {//not in L1
			if(L2_token!=-1){//find in L2
			this->myStat.accL1++;
			this->myStat.accL2++;
			this->myStat.missL1++;
			L2[adr_index][L2_token].valid=false;
			int old_lru=L2[adr_index][L2_token].lru_position;
			for(int i=0;i<8;i++){
				if((L2[adr_index][i].lru_position<old_lru)&&(L2[adr_index][i].lru_position!=-1)){
					L2[adr_index][i].lru_position++;

								}
							}
			L2[adr_index][L2_token].lru_position=-1;
			writeToL1(adr_index,adr_tag,L2[adr_index][L2_token].data,real_adr);
			}
			else{//neither in l1 or l2
				this->myStat.accL1++;
				this->myStat.accL2++;
				this->myStat.missL1++;
				this->myStat.missL2++;

				//writeToL1(adr_index,adr_tag,myMem[adr]);
			}

		}

	}

	if(MemR==true){
		if(searchL1(real_adr)==true){//search in L1
			this->myStat.accL1++;



		}

		else {
			if(L2_token!=-1){//L1 not L2 have
				this->myStat.accL1++;
				this->myStat.accL2++;
				this->myStat.missL1++;
				L2[adr_index][L2_token].valid=false;
				int old_lru=L2[adr_index][L2_token].lru_position;
				for(int i=0;i<8;i++){
					if((L2[adr_index][i].lru_position<old_lru)&&(L2[adr_index][i].lru_position!=-1)){
						L2[adr_index][i].lru_position++;

					}
				}
				L2[adr_index][L2_token].lru_position=-1;
				writeToL1(adr_index,adr_tag,myMem[adr],real_adr);



			}
			else{//neither
				this->myStat.accL1++;
				this->myStat.accL2++;
				this->myStat.missL1++;
				this->myStat.missL2++;
				writeToL1(adr_index,adr_tag,myMem[adr],real_adr);


			}



		}

	}

	// add your code here
	//test code
	//cout<<" Decimal address: "<<adr<<" Real: "<< real_adr<< " index:"<< adr_index<<" tag: "<<adr_tag<<endl;
	//cout<<"L1 miss: " << myStat.missL1 <<" L2 miss:"<< myStat.missL2<<" L1 acc: "<<myStat.accL1<<" L2 acc: "<<myStat.accL2<<endl;
	//cout<<"Memory: "<<myMem[adr]<<endl;
	//cout<<"----------------------------------------------"<<endl;


	return true;
}

vector<float> cache::return_result() {
	vector<float> result;
	result.push_back(this->myStat.missL1);
	result.push_back(this->myStat.accL1);
	result.push_back(this->myStat.missL2);
	result.push_back(this->myStat.accL2);


	return result;
}
int cache::binary_string_to_decimal(string s) {
    int decimal = 0;
    int l = s.length();
    for (int i = 0; i < l; i++) {
        if (s[i] == '1') {
            decimal += pow(2, l-i-1);
        }
    }
    return decimal;
}
