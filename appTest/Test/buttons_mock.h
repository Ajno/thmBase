/*
 * buttons_mock.h
 *
 *  Created on: 7. 10. 2015
 *      Author: z0037ysj
 */

#ifndef BUTTONS_MOCK_H_
#define BUTTONS_MOCK_H_

#include <buttons.h>

class ButtonsMock
{
public:
    ButtonsMock();
    virtual ~ButtonsMock();
    void setState(const bool cState, const buttons_t cButton);
};

#endif /* BUTTONS_MOCK_H_ */
