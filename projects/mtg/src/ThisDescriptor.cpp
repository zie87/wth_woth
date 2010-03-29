#include "../include/config.h"
#include "../include/ThisDescriptor.h"
#include "../include/Counters.h"
#include "../include/MTGCardInstance.h"
#include "../include/CardDescriptor.h"


//Returns the amount by which a value passes the comparison.
int ThisDescriptor::matchValue(int value){
  switch (comparisonMode){
    case COMPARISON_AT_MOST:
      return (comparisonCriterion - value + 1);
    case COMPARISON_AT_LEAST:
      return (value - comparisonCriterion + 1);
    case COMPARISON_EQUAL:
      return (comparisonCriterion == value);
    case COMPARISON_GREATER:
      return (value - comparisonCriterion);
    case COMPARISON_LESS:
      return (comparisonCriterion - value);
    case COMPARISON_UNEQUAL:
      return (comparisonCriterion != value);       
  }
  return 0;
}

ThisDescriptor * ThisDescriptorFactory::createThisDescriptor(string s){
  size_t found;
 
  string whitespaces (" \t\f\v\n\r");

  found=s.find_last_not_of(whitespaces);
  if (found!=string::npos)
    s.erase(found+1);
  else return NULL;

  found=s.find_first_not_of(whitespaces);
  if (found!=string::npos)
    s.erase(0,found);
  else return NULL;

  //set comparison mode
  //equal, greater, and less must remain above the others, otherwise the others may never be used.
  int mode = 0;
  size_t found2 = string::npos;
  int opLength = 0;

  found = s.find("=");
  if (found != string::npos){
    mode = COMPARISON_EQUAL;
    found2 = found + 1;
    opLength = 1;
  }
  found = s.find(">");
  if (found != string::npos){
    mode = COMPARISON_GREATER;
    found2 = found + 1;
    opLength = 1;
  }
  found = s.find("<");
  if (found != string::npos){
    mode = COMPARISON_LESS;
    found2 = found + 1; 
    opLength = 1;
  }
  found = s.find("<=");
  if (found != string::npos){
    mode = COMPARISON_AT_MOST; 
    found2 = found + 2; 
    opLength = 2;
  }
  found = s.find(">=");
  if (found != string::npos){
    mode = COMPARISON_AT_LEAST;
    found2 = found + 2;
    opLength = 2;
  }
  found = s.find("!=");
  if (found != string::npos){
    mode  = COMPARISON_UNEQUAL; 
    found2 = found + 2; 
    opLength = 2;
  }
  if (!mode) mode = COMPARISON_AT_LEAST;

  //get comparison criterion
  int criterionFound = 0;
  int criterion = 1;
  if ((found2 != string::npos) && (found2 < s.length())){
    criterion = atoi(s.substr(found2).c_str());
    criterionFound = 1;
  }
  if (found2 != string::npos) s.erase(found2 - opLength);

  //counters
  found = s.find("counter{");
  if (found != string::npos) {
    size_t start = s.find("{");
    size_t end = s.find("}");
    string counterString = s.substr(start+1,end-start-1);
    AbilityFactory * abf = NEW AbilityFactory();
    Counter * counter = abf->parseCounter(counterString,NULL);
    if (counter) {
      if (criterionFound) counter->nb = criterion;
      ThisCounter * td = NEW ThisCounter(counter);
      if (td) {
        td->comparisonMode = mode;
        return td;
      }
    }
    return NULL;
  }

  //any counter
  found = s.find("counters");
  if (found != string::npos) {
    ThisCounterAny * td = NEW ThisCounterAny(criterion);
    if (td) {
      td->comparisonMode = mode;
      return td;
    }
    return NULL;
  }

  //power
  found = s.find("power");
  if (found != string::npos) {
    ThisPower * td = NEW ThisPower(criterion);
    if (td) {
      td->comparisonMode = mode;
      return td;
    }
    return NULL;
  }

  //toughness
  found = s.find("toughness");
  if (found != string::npos) {
    ThisToughness * td = NEW ThisToughness(criterion);
    if (td) {
      td->comparisonMode = mode;
      return td;
    }
    return NULL;
  }

  return NULL;
}

ThisCounter::ThisCounter(Counter * _counter){
  counter = _counter;
  comparisonCriterion = counter->nb;
}

ThisCounter::ThisCounter(int power, int toughness, int nb, const char * name){
  counter = NEW Counter(NULL,name,power,toughness);
  comparisonCriterion = nb;
}

int ThisCounter::match(MTGCardInstance * card){
  Counter * targetCounter = card->counters->hasCounter(counter->name.c_str(),counter->power,counter->toughness);
    if (targetCounter){
      return matchValue(targetCounter->nb);
    }else{
      switch (comparisonMode) {
        case COMPARISON_LESS:
          return comparisonCriterion;
        case COMPARISON_AT_MOST:
          return comparisonCriterion + 1;
        case COMPARISON_UNEQUAL:
          if (comparisonCriterion) return 1;
          else return 0;
        case COMPARISON_EQUAL:
          if (comparisonCriterion) return 0;
          else return 1;
        default :
          return 0;
      }
    }
}

ThisCounter::~ThisCounter() {
  SAFE_DELETE(counter);
}

ThisPower::ThisPower(int power){
  comparisonCriterion = power;
}

int ThisPower::match(MTGCardInstance * card){
  return matchValue(card->power);
}

ThisToughness::ThisToughness(int toughness){
  comparisonCriterion = toughness;
}

int ThisToughness::match(MTGCardInstance * card){
  return matchValue(card->toughness);
}

ThisCounterAny::ThisCounterAny(int nb){
  comparisonCriterion = nb;
}

int ThisCounterAny::match(MTGCardInstance * card){
  int result = 0;
  for (int i = 0; i < card->counters->mCount; i++) {
    result += card->counters->counters[i]->nb;
  }
  return matchValue(result);
}