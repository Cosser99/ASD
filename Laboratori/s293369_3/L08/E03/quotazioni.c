//
// Created by simone on 08/12/22.
//

#include "quotazioni.h"

typedef struct BSTnode* link;
struct BSTnode { Quotazione item; link p; link l; link  r; int N; int NumDate;};
struct binarysearchtree { link root;  link z; };
static link NEW(Quotazione item, link p, link l, link r, int N,int NumDate);

static void treePrintR(link h, link z);
static void searchR(link h, Quotazione k, link z,BST bst);
static void searchRQuotation(link h,datetime k,link z);
static void InOrderRangePeriod(link h, link z,datetime date1,datetime date2,link *quotamax,link *quotamin,float *massimo, float *minimo);
static void checkmaxmin(link h,link **quotamax, link **quotamin, float **massimo, float **minimo);
static void InOrderAllPeriod(link h, link z,link *quotamax,link *quotamin,float *massimo, float *minimo);
static link balanceR(link h, link z);
static link partR(link h, int r);
static link rotR(link h);
static link rotL(link h);
static int AltezzaAlberoMassima(link root);
static int AltezzaAlberoMinima(link root);
static void freeR(link h, link z);

void readbst(BST *bstq,FILE *fp){
    Quotazione X;
    //(*bstq)=BSTinit();
    int dimensionitransizioni;
    fscanf(fp,"%d\n",&dimensionitransizioni);
    for(int i=0;i<dimensionitransizioni;i++) {
        X = QuotazioneScan(fp);
        BSTsearch(*bstq, X);
    }
    //treePrintR((*bstq)->root,(*bstq)->z);

}

void BSTinsert_leafI(BST bst, Quotazione x) {
    link p = bst->root, h = p;
    if (bst->root == bst->z) {
        bst->root = NEW(x, bst->z, bst->z, bst->z, x.quantitatransiozioni,1);
        return;
    }
    while (h != bst->z) {
        p = h;
        h->N++;
        h->NumDate++;
        h = (KEYcmp(KEYget(x.data), KEYget(h->item.data))==-1) ? h->l : h->r;
    }
    h = NEW(x, p, bst->z, bst->z, x.quantitatransiozioni,1);
    if (KEYcmp(KEYget(x.data), KEYget(p->item.data))==-1)
        p->l = h;
    else
        p->r = h;
}

Quotazione QuotazioneScan(FILE *fp){
    Quotazione val;
    char stringadata[11];
    fscanf(fp,"%s %*s %f %d",stringadata,&(val.valore),&(val.quantitatransiozioni));
    sscanf (stringadata,"%d/%d/%d", &val.data.anno,&val.data.mese,&val.data.giorno);
    return val;
}

BST BSTinit() {
    BST bst = malloc(sizeof *bst) ;
    bst->root = ( bst->z = NEW(QuotazionesetNull(), NULL, NULL, NULL, 0,0));
    return bst;
}

Quotazione QuotazionesetNull() {
    Quotazione val = {0,0,0,0,0};
    return val;
}

static link NEW(Quotazione item, link p, link l, link r, int N,int numdate) {
    link x = malloc(sizeof *x);
    x->item = item; x->p = p; x->l = l; x->r = r; x->N = N; x->NumDate=numdate;
    return x;
}

void QuotationStore(Quotazione val) {
    printf("year: %d month: %d day: %d value: %.2f number: %d\n", val.data.anno, val.data.mese,val.data.giorno,val.valore,val.quantitatransiozioni);
}

void treePrintR(link h, link z) {
    if (h == z) return;
    treePrintR(h->l, z);
    QuotationStore(h->item);
    treePrintR(h->r, z);
}

static void searchR(link h, Quotazione k, link z,BST bst) {
    int cmp;
    if (h == z){
        BSTinsert_leafI(bst,k);
        return;
    }
    cmp = KEYcmp(KEYget(k.data), KEYget(h->item.data));
    if (cmp==0){
        //printf("nodo gia trovato, come lo modifico?\n");
        editnodebst(&(h->item),k);
        return;
    }
    if (cmp==-1)
        return searchR(h->l, k, z,bst);
    else
        return searchR(h->r, k, z,bst);
}

void BSTsearch(BST bst, Quotazione k) {
    searchR(bst->root, k, bst->z,bst);
}

void editnodebst(Quotazione *item, Quotazione k){
    float res;
    res =  (float)(( (item->valore * (float)item->quantitatransiozioni ) + (k.valore * (float)k.quantitatransiozioni)) / ((float)item->quantitatransiozioni + (float)k.quantitatransiozioni));
    item->quantitatransiozioni=item->quantitatransiozioni+k.quantitatransiozioni;
    item->valore=res;
}

void treePrintWrapper(BST bstq){
    treePrintR(bstq->root,bstq->z);
}

void SearchBSTQuotationFromdate(BST bst,char *date){
    datetime tmp;
    sscanf (date,"%d/%d/%d",&tmp.anno,&tmp.mese,&tmp.giorno);
    searchRQuotation(bst->root,tmp,bst->z);
}

void searchRQuotation(link h,datetime k,link z){
    int cmp;
    if (h == z){
        return;
    }
    cmp = KEYcmp(KEYget(k), KEYget(h->item.data));
    if (cmp==0)
        QuotationStore(h->item);
    if (cmp==-1)
        return searchRQuotation(h->l, k, z);
    else
        return searchRQuotation(h->r, k, z);
}

void SearchBSTQuotationMINMAXFromRangeDate(BST bst,char *date1,char *date2){
    datetime tmp1;
    datetime tmp2;
    float massimo=(float)0.0;
    float minimo=(float)100000000.0;
    link quotamax=NULL;
    link quotmin=NULL;
    sscanf (date1,"%d/%d/%d",&tmp1.anno,&tmp1.mese,&tmp1.giorno);
    sscanf (date2,"%d/%d/%d",&tmp2.anno,&tmp2.mese,&tmp2.giorno);
    InOrderRangePeriod(bst->root,bst->z,tmp1,tmp2,&quotamax,&quotmin,&massimo,&minimo);
    if(quotamax!=NULL && quotmin!=NULL){
        printf("Quotazione massima: \n");
        QuotationStore(quotamax->item);
        printf("Quotazione minima: \n");
        QuotationStore(quotmin->item);
    }else{
        printf("Non c'è nessuna quotazione in questo range di date\n");
    }

}

static void InOrderRangePeriod(link h, link z,datetime date1,datetime date2,link *quotamax,link *quotamin,float *massimo, float *minimo) {

    if (h == z) return;
    InOrderRangePeriod(h->l, z,date1,date2,quotamax,quotamin,massimo,minimo);
    if(KEYcmp(KEYget(date1), KEYget(h->item.data))<=0 && KEYcmp(KEYget(h->item.data) ,KEYget(date2))<=0) checkmaxmin(h,&(quotamax),&(quotamin),&(massimo),&(minimo));
    InOrderRangePeriod(h->r, z,date1,date2,quotamax,quotamin,massimo,minimo);
}

static void checkmaxmin(link h,link **quotamax, link **quotamin, float **massimo, float **minimo){
    if((h->item.valore)>=(**massimo)){
        (**massimo)=(h->item.valore);
        (**quotamax)=h;
    }
    if(h->item.valore<=(**minimo)){
        (**minimo)=(h->item.valore);
        (**quotamin)=h;
    }
}

void SearchBSTQuotationMINMAXFromAllPeriod(BST bst){
    float massimo=(float)0.0;
    float minimo=(float)100000000.0;
    link quotamax=NULL;
    link quotmin=NULL;
    InOrderAllPeriod(bst->root,bst->z,&quotamax,&quotmin,&massimo,&minimo);
    if(quotamax!=NULL && quotmin!=NULL){
        printf("Quotazione massima: \n");
        QuotationStore(quotamax->item);
        printf("Quotazione minima: \n");
        QuotationStore(quotmin->item);
    }else{
        printf("Non c'è nessuna quotazione in questo range di date\n");
    }

}

/*
 * void treePrintR(link h, link z) {
    if (h == z) return;
    treePrintR(h->l, z);
    QuotationStore(h->item);
    treePrintR(h->r, z);
}
 */

static void InOrderAllPeriod(link h, link z,link *quotamax,link *quotamin,float *massimo, float *minimo) {

    if (h == z) return;
    InOrderAllPeriod(h->l, z,quotamax,quotamin,massimo,minimo);
    checkmaxmin(h,&(quotamax),&(quotamin),&(massimo),&(minimo));
    InOrderAllPeriod(h->r, z,quotamax,quotamin,massimo,minimo);
}

void BalanceBST(BST bst){
    bst->root=balanceR(bst->root,bst->z);
}

static link balanceR(link h, link z) {
    int r;
    if (h == NULL) return h;
    r = (h->NumDate+1)/2-1;
    h = partR(h, r);
    h->l = balanceR(h->l,z);
    h->r = balanceR(h->r,z);
    return h;
}

link partR(link h, int r) {
    if(h == NULL || (h->l == NULL && h->r == NULL)) return h;
    int t;
    if(h->l != NULL)
        t = h->l->NumDate;
    else
        t = 0;
    if ( t > r) {
        h->l = partR(h->l, r);
        h = rotR(h);
    }
    if ( t < r) {
        h->r = partR(h->r, r-t-1);
        h = rotL(h);
    }
    return h;
}

link rotR(link h) {
    link x = h->l;
    h->l = x->r;
    x->r = h;
    x->NumDate = h->NumDate;
    //x->N=h->N;
    h->NumDate = 1;
    h->NumDate += (h->l) ? h->l->NumDate : 0;
    h->NumDate += (h->r) ? h->r->NumDate : 0;
    return x;
}

link rotL(link h) {
    link x = h->r;
    h->r = x->l;
    x->l = h;
    x->NumDate = h->NumDate;
    h->NumDate = 1;
    h->NumDate += (h->l) ? h->l->NumDate : 0;
    h->NumDate += (h->r) ? h->r->NumDate : 0;
    return x;
}

int AltezzaAlberoMassimaWrapper(BST bst){
    return AltezzaAlberoMassima(bst->root);
}

static int AltezzaAlberoMassima(link root){
    int u, v;
    if (root == NULL) return 0;
    if (root->l==NULL && root->r==NULL) return 1;

    u = AltezzaAlberoMassima(root->l);
    v = AltezzaAlberoMassima(root->r);

    if (u>v){
        //printf("Boh :%d\n",u+1);
        return u+1;
    }
    //printf("Boh2 :%d\n",v+1);

    return v+1;

}

int AltezzaAlberoMinimaWrapper(BST bst){
    return AltezzaAlberoMinima(bst->root);
}

static int AltezzaAlberoMinima(link root){
    int u, v;
    if (root == NULL) return 0;
    if (root->l==NULL && root->r==NULL) return 1;

    u = AltezzaAlberoMinima(root->l);
    v = AltezzaAlberoMinima(root->r);

    if (u<=v){
        //printf("Boh :%d\n",u+1);
        return u+1;
    }
    //printf("Boh2 :%d\n",v+1);

    return v+1;

}

static void freeR(link h, link z){
    if(h == z)
        return;
    freeR(h->l, z);
    freeR(h->r, z);
    free(h);
}
void BSTQUOTfree(BST bst){
    freeR(bst->root, bst->z);
}

