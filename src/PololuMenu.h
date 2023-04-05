// Copyright Pololu Corporation.  For more information, see http://www.pololu.com/

/// \file PololuMenuMain.h

#pragma once

#include <Arduino.h>
#include <Pushbutton.h>
#include "Pololu3piPlus2040Buzzer.h"
#include "Pololu3piPlus2040OLED.h"

struct PololuMenuItem
{
  const char* name;
  void (* action)();
};

class PololuMenu
{
public:
  static const char beepPrevious[];
  static const char beepSelect[];
  static const char beepNext[];

  void restart()
  {
    lcdItemIndex = 0;
    lcdNeedsUpdate = true;
  }

  void setItems(const PololuMenuItem * items, uint8_t itemCount)
  {
    this->items = items;
    this->itemCount = itemCount;
  }

  void setDisplay(Pololu3piPlus2040::OLED & l)
  {
    lcd = &l;
  }

  void setBuzzer(Pololu3piPlus2040::Buzzer & b)
  {
    buzzer = &b;
  }

  void setPreviousButton(PushbuttonBase & b, char name)
  {
    previousButton = &b;
    previousButtonName = name;
  }

  void setSelectButton(PushbuttonBase & b, char name)
  {
    selectButton = &b;
    selectButtonName = name;
  }

  void setNextButton(PushbuttonBase & b, char name)
  {
    nextButton = &b;
    nextButtonName = name;
  }

  void setButtons(PushbuttonBase & previous, PushbuttonBase & select, PushbuttonBase & next)
  {
    setPreviousButton(previous, 'A');
    setSelectButton(select, 'B');
    setNextButton(next, 'C');
  }

  void lcdUpdate(uint8_t index)
  {
    if (lcd == NULL) { return; }
    lcd->clear();
    lcd->print(items[index].name);
    lcd->gotoXY(0, 1);
    if (secondLine != NULL)
    {
      lcd->print(secondLine);
    }
    else
    {
      // Generate something based on the key names
      if (previousButton)
      {
        lcd->print('<');
        lcd->print(previousButtonName);
      }
      if (selectButton)
      {
        lcd->gotoXY(3,1);
        lcd->print('*');
        lcd->print(selectButtonName);
      }
      if (nextButton)
      {
        lcd->gotoXY(6,1);
        lcd->print(nextButtonName);
        lcd->print('>');
      }
    }
  }

  void action(uint8_t index)
  {
    items[index].action();
  }

  // This function is meant to be called repeatedly in a loop.
  // Prompts the user to choose one of the menu items.
  // If a button is pressed, take the appropriate action;
  // return true if a menu item was selected.
  bool select()
  {
    if(lcdNeedsUpdate)
    {
      lcdUpdate(lcdItemIndex);
    }

    // Assume we will need to update, but set to false if
    // there are no button events.
    lcdNeedsUpdate = true;

    char button = buttonMonitor();
    if(button && button == previousButtonName)
    {
      // The "previous" button was pressed so decrement the index.
      if (lcdItemIndex == 0)
      {
        lcdItemIndex = itemCount - 1;
      }
      else
      {
        lcdItemIndex--;
      }
      return false;
    }

    if(button && button == nextButtonName)
    {
      // The "next" button was pressed so increase the index.
      if (lcdItemIndex >= itemCount - 1)
      {
        lcdItemIndex = 0;
      }
      else
      {
        lcdItemIndex++;
      }
      return false;
    }

    if(button && button == selectButtonName)
    {
      // The "select" button was pressed so run the item and return true.
      action(lcdItemIndex);
      return true;
    }

    // Nothing happened, no update necessary
    lcdNeedsUpdate = false;
    return false;
  }


  // This function watches for button presses.  If a button is
  // pressed, it beeps a corresponding beep and it returns 'A',
  // 'B', or 'C' depending on what button was pressed.  If no
  // button was pressed, it returns 0.  This function is meant to
  // be called repeatedly in a loop.
  char buttonMonitor()
  {
    if ((previousButton != NULL) && previousButton->getSingleDebouncedPress())
    {
      if(buzzer != NULL)
        buzzer->play(beepPrevious);
      return previousButtonName;
    }

    if ((selectButton != NULL) && selectButton->getSingleDebouncedPress())
    {
      if(buzzer != NULL)
        buzzer->play(beepSelect);
      return selectButtonName;
    }

    if ((nextButton != NULL) && nextButton->getSingleDebouncedPress())
    {
      if(buzzer != NULL)
        buzzer->play(beepNext);
      return nextButtonName;
    }

    return 0;
  }

  // Set the second line of text to be displayed below each menu
  // option.  By default it displays "<A .B C>".  To replace the
  // special characters with nicer arrows and a dot, you could
  // use "\x7f" "A \xa5" "B C\x7e".
  void setSecondLine(const char* text)
  {
    secondLine = text;
  }

private:
  const PololuMenuItem * items;
  uint8_t itemCount;
  uint8_t lcdItemIndex;
  Pololu3piPlus2040::OLED * lcd;
  Pololu3piPlus2040::Buzzer * buzzer;
  PushbuttonBase * previousButton;
  PushbuttonBase * selectButton;
  PushbuttonBase * nextButton;

  char previousButtonName;
  char selectButtonName;
  char nextButtonName;

  const char * secondLine;

  bool lcdNeedsUpdate = true;
};

const char PololuMenu::beepPrevious[] = "!c32";
const char PololuMenu::beepSelect[] = "!e32";
const char PololuMenu::beepNext[] = "!g32";
