#include "sketchwidget.h"
#include <stdio.h>

SketchWidget::SketchWidget(QWidget *parent) :
    QWidget(parent)
{
    this->setMouseTracking(true);
    mouseButtonPressed = false;
    currentLine = false;
    treeEmpty = true;
    newBranch = NewBranch();

    thickVar = false;
    thickVarType = "NULL";
    lengthVar = false;
    lengthVarType = "NULL";
    angleVar = false;
    angleVarType = "NULL";
    segThickVar = false;
    segAngleVar = false;
    segLengthVar = false;

    this->activateWindow();
    this->setFocus();
    identifierIncrementer = 1;
    toggleAccentSelected = false;
    isBranchSelected = false;
    lineMode = true;
    pencilMode = false;
    selectMode = false;
    sb = SelectBox();

    ctrlKeyDown = false;

    bp = BranchPlacement();

    mouseInputAlreadyAccepted = false;

    defaultThickness = 3.0f;

    panDiff = QPointF(0.0f, 0.0f);
    isPan = false;

    sketchEmpty();
    sketchChanged();
}

void SketchWidget::clear()
{
}

void SketchWidget::mousePressEvent(QMouseEvent *event)
{
    Qt::MouseButtons mb = event->buttons();
    if (!isPan && !mouseButtonPressed)
    {
        if (mb == Qt::LeftButton)
        {
            startPos = event->posF();
            mouseButtonPressed = true;
            endPos = startPos;
            prevMousePos = startPos;

            if (!treeEmpty)
            {
                if (isBranchSelected)
                    checkVariation(tree, event->posF() - panDiff);
            }
            if (!thickVar && !lengthVar && !angleVar && !segAngleVar && !segThickVar && !segLengthVar)
            {
                currentLine = true;
                if (pencilMode)
                {
                    segBranchTimer.start();
                    newBranchSegs.clear();
                    currentSegStartPos = startPos;
                }
            }
        }
        else
        {
            isPan = true;
            startPos = event->posF();
            prevMousePos = startPos;
            setCursor(QCursor(Qt::ClosedHandCursor));
        }
    }
}

void SketchWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (!treeEmpty)
    {
        Qt::MouseButtons mb = event->buttons();
        endPos = event->posF();
        if (mb == Qt::LeftButton)
        {
            if (!ctrlKeyDown)
            {
                deselectBranch(tree);
            }

            smallDist = INT_MAX;
            SelectBranch(endPos - panDiff, tree);
            // Check distance less than 30
            if (smallDist < 30)
            {
                // Select node
                if (closestBranch->getIsTrunk() && !closestBranch->getIsSegment())
                {
                    closestBranch->setIsSelected(true);
                }
                else
                {
                    if (closestBranch->getIsSegment() && !closestBranch->getIsRootSegment())
                    {
                        for (int i = 0 ; i < segBranches.size() ; i++)
                            if (segBranches.value(i).hasMember(closestBranch->getIdentifier()))
                                findRootSegment(tree, segBranches.value(i).getAllIdentifiers().at(0));
                    }
                    if (closestBranch->getIsTrunk() && closestBranch->getIsSegment())
                    {
                        for (int i = 0 ; i < segBranches.size() ; i++)
                            if (segBranches.value(i).hasMember(closestBranch->getIdentifier()))
                            {
                                QVector<long> tempIdentifiers = segBranches.value(i).getAllIdentifiers();
                                for (int j = 0 ; j < tempIdentifiers.size() ; j++)
                                    setBranchStatus(tempIdentifiers.value(j), tree, true);
                            }
                    }
                    else if (closestBranch->getParent()->getIsSegment())
                    {
                        for (int i = 0 ; i < segBranches.size() ; i++)
                            if (segBranches.value(i).hasMember(closestBranch->getParent()->getIdentifier()))
                            {
                                QVector<long> tempIdentifiers = segBranches.value(i).getAllIdentifiers();
                                for (int j = 0 ; j < tempIdentifiers.size() ; j++)
                                {
                                    findRootSegment(tree, tempIdentifiers.value(j));
                                    for (int k = 0 ; k < closestBranch->numChildren() ; k++)
                                        if (closestBranch->childAt(k)->getIsRootSegment() || !closestBranch->childAt(k)->getIsSegment())
                                        {
                                            closestBranch->childAt(k)->setIsSelected(true);
                                            if (closestBranch->childAt(k)->getIsSegment())
                                                for (int l = 0 ; l < segBranches.size() ; l++)
                                                    if (segBranches.value(l).hasMember(closestBranch->childAt(k)->getIdentifier()))
                                                    {
                                                        QVector<long> tempIdentifiers2 = segBranches.value(l).getAllIdentifiers();
                                                        for (int m = 0 ; m < tempIdentifiers2.size() ; m++)
                                                            setBranchStatus(tempIdentifiers2.value(m), tree, true);
                                                    }
                                        }
                                }
                            }
                    }
                    else
                    {
                        for (int i = 0 ; i < closestBranch->getParent()->numChildren() ; i++)
                        {
                            closestBranch->getParent()->childAt(i)->setIsSelected(true);
                            if (closestBranch->getParent()->childAt(i)->getIsSegment())
                                for (int j = 0 ; j < segBranches.size() ; j++)
                                    if (segBranches.value(j).hasMember(closestBranch->getParent()->childAt(i)->getIdentifier()))
                                    {
                                        QVector<long> tempIdentifiers = segBranches.value(j).getAllIdentifiers();
                                        for (int k = 0 ; k < tempIdentifiers.size() ; k++)
                                            setBranchStatus(tempIdentifiers.value(k), tree, true);
                                    }
                        }
                    }
                }
            }
        }
        mouseInputAlreadyAccepted = true;
    }
    update();
}

void SketchWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (isPan)
    {
        endPos = event->posF();
        panDiff += endPos - prevMousePos;
        prevMousePos = event->posF();
    }
    else
    {
        if (!treeEmpty)
        {
            if (mouseButtonPressed)
                drawClosePoint = bp.drawClosePoint(tree, startPos - panDiff, event->posF() - panDiff);
            else
                drawClosePoint = bp.drawClosePoint(tree, event->posF() - panDiff, event->posF() - panDiff);
        }

        if (mouseButtonPressed)
        {
            endPos = event->posF();
            if (thickVar)
            {
                branchBeingModified->modifyWidthVariation(endPos - panDiff, prevMousePos - panDiff, thickVarType);
                float widthChange = branchBeingModified->getWidthVariation(thickVarType);
                applyWidthChangeToOther(widthChange, thickVarType);
            }
            if (lengthVar)
            {
                branchBeingModified->modifyLengthVariation(endPos - panDiff, lengthVarType);
                float lengthChange = branchBeingModified->getLengthVariation(lengthVarType);
                applyLengthChangeToOther(lengthChange, lengthVarType);
            }
            if (angleVar)
            {
                branchBeingModified->modifyAngleVariation(endPos - panDiff);
                float angleChange = QLineF(branchBeingModified->getStartPos(), branchBeingModified->getLengthMaxPoint()).angleTo(QLineF(branchBeingModified->getStartPos(), branchBeingModified->getAngleVariation(angleVarType)));
                if (angleChange > 180.0f)
                    angleChange = 360 - angleChange;
                applyAngleChangeToOther(angleChange, angleVarType);
            }
            if (segAngleVar)
            {
                segBranches.value(findSegmentedBranchIndex(branchBeingModifiedIdentifier)).modifyAngleVariation(tree, endPos - panDiff);
                float angleChange = QLineF(branchBeingModified->getStartPos(), branchBeingModified->getLengthMaxPoint()).angleTo(QLineF(branchBeingModified->getStartPos(), branchBeingModified->getAngleVariation(angleVarType)));
                if (angleChange > 180.0f)
                    angleChange = 360 - angleChange;
                applyAngleChangeToOther(angleChange, angleVarType);
            }
            if (segThickVar)
            {
                SegmentedBranch tempSegBranch = segBranches.value(findSegmentedBranchIndex(branchBeingModifiedIdentifier));
                tempSegBranch.modifyWidthVariation(tree, endPos - panDiff, prevMousePos - panDiff, thickVarType);
                float widthChange = tempSegBranch.getWidthVariation(this->tree, thickVarType);
                applyWidthChangeToOther(widthChange, thickVarType);
                tree = tempSegBranch.getTree();
                segBranches.replace(findSegmentedBranchIndex(branchBeingModifiedIdentifier), tempSegBranch);
                // Modify segment thickness here
                // get length of thickness
                // apply to all other branches
            }
            if (segLengthVar)
            {
                SegmentedBranch tempSegBranch = segBranches.value(findSegmentedBranchIndex(branchBeingModifiedIdentifier));
                tempSegBranch.modifyLengthVariation(tree, endPos - panDiff, lengthVarType);
                float lengthChange = tempSegBranch.getLengthVariation(this->tree, lengthVarType);
                applyLengthChangeToOther(lengthChange, lengthVarType);
                tree = tempSegBranch.getTree();
                segBranches.replace(findSegmentedBranchIndex(branchBeingModifiedIdentifier), tempSegBranch);
            }
            prevMousePos = event->posF();
            if (pencilMode)
            {
                if (segBranchTimer.elapsed() >= 100)
                {
                    if (FindDistance(currentSegStartPos, event->posF()) > 30)
                    {
                        QPointF currentSegEndPos = event->posF();
                        newBranchSegs.append(QLineF(currentSegStartPos, currentSegEndPos));
                        currentSegStartPos = currentSegEndPos;
                    }
                    segBranchTimer.restart();
                }
            }
        }
    }
    update();
}

void SketchWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (isPan)
    {
        //endPos = event->posF();
        //panDiff = endPos - startPos;

        if (lineMode || selectMode)
        {
            setCursor(QCursor(QPixmap("./Resources/Cursors/Precision.png"),16,16));
        }
        else if (pencilMode)
        {
            setCursor(QCursor(QPixmap("./Resources/Cursors/Handwrite.png"),0,32));
        }
        else
        {
            setCursor(QCursor(Qt::ArrowCursor));
        }
    }
    else if (mouseButtonPressed)
    {
        if (!mouseInputAlreadyAccepted)
        {
            endPos = event->posF();
            currentLine = false;
            mouseButtonPressed = false;
            double distMoved = FindDistance(startPos, endPos);

            if ((distMoved < 5.0f && !pencilMode) || (distMoved < 5.0f && pencilMode && newBranchSegs.size() < 1))
            {
                if (!treeEmpty && !angleVar && !lengthVar && !thickVar && !segAngleVar && !segLengthVar && !segThickVar)
                {
                    if (!ctrlKeyDown)
                    {
                        deselectBranch(tree);
                    }

                    smallDist = INT_MAX;
                    SelectBranch(endPos - panDiff, tree);
                    // Check distance less than 30
                    if (smallDist < 30)
                    {
                        // Select node
                        if (closestBranch->getIsSelected())
                        {
                            closestBranch->setIsSelected(false);
                            if (closestBranch->getIsSegment())
                                for (int i = 0 ; i < segBranches.size() ; i++)
                                    if (segBranches.value(i).hasMember(closestBranch->getIdentifier()))
                                    {
                                        QVector<long> tempIdentifiers = segBranches.value(i).getAllIdentifiers();
                                        for (int j = 0 ; j < tempIdentifiers.size() ; j++)
                                            setBranchStatus(tempIdentifiers.value(j), tree, false);
                                    }
                        }
                        else
                        {
                            closestBranch->setIsSelected(true);
                            if (closestBranch->getIsSegment())
                                for (int i = 0 ; i < segBranches.size() ; i++)
                                    if (segBranches.value(i).hasMember(closestBranch->getIdentifier()))
                                    {
                                        QVector<long> tempIdentifiers = segBranches.value(i).getAllIdentifiers();
                                        for (int j = 0 ; j < tempIdentifiers.size() ; j++)
                                            setBranchStatus(tempIdentifiers.value(j), tree, true);
                                    }
                        }

                        if (checkForSelectedBranch(tree))
                            isBranchSelected = true;
                        else
                            isBranchSelected = false;
                    }

                    else if (!thickVar && !lengthVar && !angleVar && !segAngleVar && !segThickVar && !segLengthVar)
                    {
                        deselectBranch(tree);
                        isBranchSelected = false;
                    }
                }
            }
            else
            {
                // Adding a branch in line mode
                if (!thickVar && !lengthVar && !angleVar && !segAngleVar && !segThickVar && !segLengthVar && lineMode)
                {
                    if (!treeEmpty)
                        deselectBranch(tree);

                    ChangeAddBranch* addBranch = new ChangeAddBranch(startPos - panDiff, endPos - panDiff, tree, treeEmpty, identifierIncrementer, defaultThickness);
                    addBranch->apply();
                    tree = addBranch->getTree();
                    treeEmpty = false;
                    drawClosePoint = bp.drawClosePoint(tree, endPos - panDiff, endPos - panDiff);
                    changeStack.push(addBranch);
                    identifierIncrementer++;
                    redoStack.clear();
                    sketchNonEmpty();
                    sketchChanged();
                }
                // Adding a branch in pencil mode
                if (!thickVar && !lengthVar && !angleVar && !segAngleVar && !segThickVar && !segLengthVar && pencilMode)
                {
                    if (FindDistance(currentSegStartPos, endPos) > 5.0f)
                        newBranchSegs.append(QLineF(currentSegStartPos, endPos));

                    if (newBranchSegs.size() >= 1)
                    {
                        newBranchSegs = applyPanDiffToBranchSegs(newBranchSegs, panDiff);
                        ChangeAddSegmentedBranch* addSegBranch = new ChangeAddSegmentedBranch(newBranchSegs, tree, treeEmpty, identifierIncrementer, defaultThickness);
                        addSegBranch->apply();
                        tree = addSegBranch->getTree();
                        treeEmpty = false;
                        drawClosePoint = bp.drawClosePoint(tree, endPos - panDiff, endPos - panDiff);
                        changeStack.push(addSegBranch);

                        SegmentedBranch tempSegBranch = SegmentedBranch();
                        for (int i = 0 ; i < newBranchSegs.size() ; i++)
                        {
                            tempSegBranch.addSegmentId(tree, identifierIncrementer);
                            identifierIncrementer++;
                        }
                        tempSegBranch.setLengthBranchPercentages(tree);
                        segBranches.append(tempSegBranch);
                    }
                    redoStack.clear();
                    newBranchSegs.clear();
                    sketchNonEmpty();
                    sketchChanged();
                }
                // Selecting branches in Select Box
                if (!thickVar && !lengthVar && !angleVar && !segAngleVar && !segThickVar && !segLengthVar && !treeEmpty && selectMode)
                {
                    if (!ctrlKeyDown)
                        deselectAll();
                    QRectF selectRect = QRectF(startPos - panDiff, endPos - panDiff);
                    tree = sb.useSelectBox(tree, selectRect, segBranches);
                    if (checkForSelectedBranch(tree))
                        isBranchSelected = true;
                }
            }

            if (thickVar)
            {
                // Branch is being modified, find branch and final branch length and create change for it
                finalVariationLength = branchBeingModified->getWidthVariation(thickVarType);

                float tempSMin = branchBeingModified->getWidthVariation("smin");
                float tempSMax = branchBeingModified->getWidthVariation("smax");
                float tempEMin = branchBeingModified->getWidthVariation("emin");
                float tempEMax = branchBeingModified->getWidthVariation("emax");
                currBranchWidthFinal.setVariation(tempSMin, tempSMax, tempEMin, tempEMax);

                for (int i = 0 ; i < branchesBeingModifiedIdentifier.size() ; i++)
                    setFinalWidthVariations(this->tree, branchesBeingModifiedIdentifier.at(i), thickVarType);
                ChangeWidthVariation* widthVariationBranch = new ChangeWidthVariation(currBranchWidthOrig, currBranchWidthFinal, branchesBeingModifiedIdentifier, origWidthChanges, finalWidthChanges, branchBeingModifiedIdentifier, tree);
                widthVariationBranch->apply();
                tree = widthVariationBranch->getTree();
                changeStack.push(widthVariationBranch);
                redoStack.clear();
                sketchChanged();
            }
            if (lengthVar)
            {
                finalVariationLength = branchBeingModified->getLengthVariation(lengthVarType);
                for (int i = 0 ; i < branchesBeingModifiedIdentifier.size() ; i++)
                    setFinalLengthVariations(this->tree, branchesBeingModifiedIdentifier.at(i), lengthVarType);
                ChangeLengthVariation* lengthVariationBranch = new ChangeLengthVariation(origVariationLength, finalVariationLength, branchesBeingModifiedIdentifier, origVariationLengths, finalVariationLengths, lengthVarType, branchBeingModifiedIdentifier, tree);
                lengthVariationBranch->apply();
                tree = lengthVariationBranch->getTree();
                changeStack.push(lengthVariationBranch);
                redoStack.clear();
                sketchChanged();
            }
            if (angleVar)
            {
                finalVariationPoint = branchBeingModified->getAngleVariation(angleVarType);
                for (int i = 0 ; i < branchesBeingModifiedIdentifier.size() ; i++)
                    setFinalAngleVariations(this->tree, branchesBeingModifiedIdentifier.at(i), angleVarType);
                ChangeAngleVariation* angleVariationBranch = new ChangeAngleVariation(origVariationPoint, finalVariationPoint, branchesBeingModifiedIdentifier, origVariationPoints, finalVariationPoints, angleVarType, branchBeingModifiedIdentifier, tree);
                angleVariationBranch->apply();
                tree = angleVariationBranch->getTree();
                changeStack.push(angleVariationBranch);
                redoStack.clear();
                sketchChanged();
            }
            if (segThickVar)
            {
                finalVariationLength = branchBeingModified->getWidthVariation(thickVarType);

                float tempSMin = branchBeingModified->getWidthVariation("smin");
                float tempSMax = branchBeingModified->getWidthVariation("smax");
                float tempEMin = branchBeingModified->getWidthVariation("emin");
                float tempEMax = branchBeingModified->getWidthVariation("emax");
                currBranchWidthFinal.setVariation(tempSMin, tempSMax, tempEMin, tempEMax);

                for (int i = 0 ; i < branchesBeingModifiedIdentifier.size() ; i++)
                    setFinalWidthVariations(this->tree, branchesBeingModifiedIdentifier.value(i), thickVarType);

                ChangeWidthVariation* widthVariationBranch = new ChangeWidthVariation(currBranchWidthOrig, currBranchWidthFinal, branchesBeingModifiedIdentifier, origWidthChanges, finalWidthChanges, branchBeingModifiedIdentifier, tree);
                widthVariationBranch->apply();
                tree = widthVariationBranch->getTree();
                changeStack.push(widthVariationBranch);
                redoStack.clear();
                sketchChanged();
            }
            if (segAngleVar)
            {
                finalVariationPoint = branchBeingModified->getAngleVariation(angleVarType);
                for (int i = 0 ; i < branchesBeingModifiedIdentifier.size() ; i++)
                    setFinalAngleVariations(this->tree, branchesBeingModifiedIdentifier.at(i), angleVarType);
                ChangeAngleVariation* angleVariationBranch = new ChangeAngleVariation(origVariationPoint, finalVariationPoint, branchesBeingModifiedIdentifier, origVariationPoints, finalVariationPoints, angleVarType, branchBeingModifiedIdentifier, tree);
                angleVariationBranch->apply();
                tree = angleVariationBranch->getTree();
                changeStack.push(angleVariationBranch);
                redoStack.clear();
                sketchChanged();
            }
            if (segLengthVar)
            {
                finalVariationLength = branchBeingModified->getLengthVariation(lengthVarType);
                for (int i = 0 ; i < branchesBeingModifiedIdentifier.size() ; i++)
                    setFinalLengthVariations(this->tree, branchesBeingModifiedIdentifier.at(i), lengthVarType);
                ChangeLengthVariation* lengthVariationBranch = new ChangeLengthVariation(origVariationLength, finalVariationLength, branchesBeingModifiedIdentifier, origVariationLengths, finalVariationLengths, lengthVarType, branchBeingModifiedIdentifier, tree);
                lengthVariationBranch->apply();
                tree = lengthVariationBranch->getTree();
                changeStack.push(lengthVariationBranch);
                redoStack.clear();
                sketchChanged();
            }
        }
    }

    thickVar = false;
    lengthVar = false;
    angleVar = false;

    segThickVar = false;
    segAngleVar = false;
    segLengthVar = false;

    newBranchSegs.clear();
    mouseInputAlreadyAccepted = false;

    isPan = false;

    update();

}

void SketchWidget::deselectBranch(Branch* tr)
{
    tr->setIsSelected(false);
    for (int i = 0 ; i < tr->numChildren() ; i++)
        deselectBranch(tr->childAt(i));
}

void SketchWidget::selectAllBranches(Branch* tr)
{
    tr->setIsSelected(true);
    for (int i = 0 ; i < tr->numChildren() ; i++)
        selectAllBranches(tr->childAt(i));
}

float SketchWidget::FindDistance(QPointF a, QPointF b)
{
    int x = (a.x() - b.x()) * (a.x() - b.x());
    int y = (a.y() - b.y()) * (a.y() - b.y());
    float dist = sqrt(x + y);
    return dist;
}

void SketchWidget::SelectBranch(QPointF nb, Branch* tr)
{
    QPointF closePoint = FindPoint(tr->getStartPos(), tr->getLengthMaxPoint(), nb);
    float distance = FindDistance(closePoint, nb);
    if (distance < smallDist)
    {
        smallDist = distance;
        closestBranch = tr;
    }
    for (int i = 0 ; i < tr->numChildren() ; i++)
    {
        SelectBranch(nb, tr->childAt(i));
    }
}

QPointF SketchWidget::FindPoint(QPointF a, QPointF b, QPointF c)
{
    QPointF ac = c-a;
    QPointF ab = b-a;
    float ab2 = (ab.x() * ab.x()) + (ab.y() * ab.y());
    float ac_ab = (ac.x() * ab.x()) + (ac.y() * ab.y());
    float t = ac_ab / ab2;
    if (t < 0.0f)
        t = 0.0f;
    else if (t > 1.0f)
        t = 1.0f;
    QPointF closest = a + (ab * t);
    return closest;
}

void SketchWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillRect(event->rect(), Qt::white);

    pen.setWidth(2);
    pen.setColor(Qt::red);

    painter.setPen(pen);

    // Draw current line in red
    if (currentLine && lineMode)
    {
        painter.setBrush(QBrush(Qt::red, Qt::SolidPattern));
        drawFirstCurrentLine(&painter, QLineF(startPos, endPos), defaultThickness);
    }
    if (currentLine && pencilMode)
    {
        painter.setBrush(QBrush(Qt::red, Qt::SolidPattern));

        QPointF* topRectanglePoints;
        for (int i = 0 ; i < newBranchSegs.size() ; i++)
        {
            if (i == 0)
                topRectanglePoints = drawFirstCurrentLine(&painter, QLineF(newBranchSegs[i].p1(), newBranchSegs[i].p2()), defaultThickness);
            else
                topRectanglePoints = drawFollowingCurrentLine(&painter, QLineF(newBranchSegs[i].p1(), newBranchSegs[i].p2()), defaultThickness, topRectanglePoints);
        }

        if (newBranchSegs.empty())
            drawFirstCurrentLine(&painter, QLineF(currentSegStartPos, endPos), defaultThickness);
        else
            drawFollowingCurrentLine(&painter, QLineF(currentSegStartPos, endPos), defaultThickness, topRectanglePoints);
    }
    if (currentLine && selectMode)
    {
        pen.setWidth(2);
        pen.setColor(Qt::red);
        painter.setPen(pen);

        QRectF drawRect = QRectF(startPos, endPos);
        painter.drawRect(drawRect);
    }

    // Traverse tree and draw existing branches
    pen.setColor(Qt::darkGreen);
    painter.setPen(pen);

    if (!treeEmpty)
    {
        paintTree(&painter, tree, false);
        paintTree(&painter, tree, true);
        paintVariation(&painter, tree);
    }

    // Draw closestPoint

    pen.setColor(Qt::blue);
    pen.setWidth(2);
    painter.setBrush(QBrush(Qt::blue, Qt::SolidPattern));
    painter.setPen(pen);
    if (!treeEmpty && !lengthVar && !angleVar && !thickVar && !segAngleVar && !segLengthVar && !segThickVar && !(mouseButtonPressed && selectMode))
        painter.drawEllipse(drawClosePoint + panDiff, 3, 3);

    painter.end();
}

void SketchWidget::paintTree(QPainter* painter, Branch* tree, bool drawSelected)
{
    pen.setColor(Qt::darkBlue);
    painter->setPen(pen);
    if (!tree->getIsSegment())
        tree->drawLine(painter, panDiff);
    else
        if (tree->getIsRootSegment())
            for (int i = 0 ; i < segBranches.size() ; i++)
                if (segBranches.value(i).hasMember(tree->getIdentifier()))
                {
                    SegmentedBranch tempSegBranch = segBranches.value(i);
                    if (drawSelected && tree->getIsSelected())
                        tempSegBranch.drawBranch(tree, painter, true, panDiff);
                    else
                        if (!drawSelected && !tree->getIsSelected())
                            tempSegBranch.drawBranch(tree, painter, false, panDiff);
                    segBranches.replace(i, tempSegBranch);
                }
    for (int i = 0 ; i < tree->numChildren() ; i++)
        paintTree(painter, tree->childAt(i), drawSelected);
}

void SketchWidget::paintVariation(QPainter* painter, Branch* tree)
{
    if (tree->getIsSelected())
    {
        if (!tree->getIsSegment())
            tree->drawVariation(painter, panDiff);
        else if (tree->getIsRootSegment())
            for (int i = 0 ; i < segBranches.size() ; i++)
                if (segBranches.value(i).hasMember(tree->getIdentifier()))
                {
                    SegmentedBranch tempSegBranch = segBranches.value(i);
                    tempSegBranch.drawVariation(this->tree, painter, panDiff);
                }
    }

    if (tree->getIsSelected() && !tree->getIsSegment())
        tree->drawVariation(painter, panDiff);

    for (int i = 0 ; i < tree->numChildren() ; i++)
        paintVariation(painter, tree->childAt(i));
}

void SketchWidget::checkVariation(Branch* tree, QPointF mousePoint)
{
    float distToLength = INT_MAX;
    float distToAngle = INT_MAX;
    float distToWidth = INT_MAX;
    if (tree->getIsSelected() && !tree->getIsSegment())
    {
        if (tree->checkOnLine(mousePoint).compare("NULL") != 0)
            distToLength = tree->getDistanceToLengthWidget();
        if (tree->checkOnPoint(mousePoint).compare("NULL") != 0)
            distToWidth = tree->getDistanceToThicknessWidget();
        if (tree->checkOnLineA(mousePoint).compare("NULL") != 0)
            distToAngle = tree->getDistanceToAngleWidget();
    }

    if (distToLength < distToAngle && distToLength < distToWidth)
    {
        lengthVar = true;
        lengthVarType = tree->checkOnLine(mousePoint);
        branchBeingModified = tree;
        branchBeingModifiedIdentifier = branchBeingModified->getIdentifier();
        origVariationLength = branchBeingModified->getLengthVariation(lengthVarType);
        branchesBeingModifiedIdentifier.clear();
        origVariationLengths.clear();
        finalVariationLengths.clear();
        findOtherSelectedBranches(this->tree, lengthVarType, "length");
    }
    if (distToAngle < distToLength && distToAngle < distToWidth)
    {
        angleVar = true;
        angleVarType = tree->checkOnLineA(mousePoint);
        branchBeingModified = tree;
        branchBeingModifiedIdentifier = branchBeingModified->getIdentifier();
        origVariationPoint = branchBeingModified->getAngleVariation(angleVarType);
        branchesBeingModifiedIdentifier.clear();
        origVariationPoints.clear();
        finalVariationPoints.clear();
        findOtherSelectedBranches(this->tree, angleVarType, "angle");
    }
    if (distToWidth < distToAngle && distToWidth < distToLength)
    {
        thickVar = true;
        thickVarType = tree->checkOnPoint(mousePoint);
        branchBeingModified = tree;
        branchBeingModifiedIdentifier = branchBeingModified->getIdentifier();
        origVariationLength = branchBeingModified->getWidthVariation(thickVarType);
        currBranchWidthOrig = WidthChanges();
        float tempSMin = branchBeingModified->getWidthVariation("smin");
        float tempSMax = branchBeingModified->getWidthVariation("smax");
        float tempEMin = branchBeingModified->getWidthVariation("emin");
        float tempEMax = branchBeingModified->getWidthVariation("emax");
        currBranchWidthOrig.setVariation(tempSMin, tempSMax, tempEMin, tempEMax);
        branchesBeingModifiedIdentifier.clear();
        origVariationLengths.clear();
        finalVariationLengths.clear();
        origWidthChanges.clear();
        finalWidthChanges.clear();
        findOtherSelectedBranches(this->tree, thickVarType, "width");
    }

    distToLength = INT_MAX;
    distToAngle = INT_MAX;
    distToWidth = INT_MAX;

    if (tree->getIsSelected() && tree->getIsSegment())
    {
        if (segBranches.value(findSegmentedBranchIndex(tree->getIdentifier())).lengthCheckOnPoint(this->tree, mousePoint).compare("NULL") != 0)
            distToLength = segBranches.value(findSegmentedBranchIndex(tree->getIdentifier())).getDistanceToLengthWidget();
        if (segBranches.value(findSegmentedBranchIndex(tree->getIdentifier())).widthCheckOnPoint(this->tree, mousePoint).compare("NULL") != 0)
        {
            SegmentedBranch tempBranch = segBranches.value(findSegmentedBranchIndex(tree->getIdentifier()));
            tempBranch.widthCheckOnPoint(this->tree, mousePoint);
            segBranches.replace(findSegmentedBranchIndex(tree->getIdentifier()), tempBranch);
            distToWidth = segBranches.value(findSegmentedBranchIndex(tree->getIdentifier())).getDistanceToThicknessWidget();
        }
        if (segBranches.value(findSegmentedBranchIndex(tree->getIdentifier())).angleCheckOnLine(this->tree, mousePoint).compare("NULL") != 0)
            distToAngle = segBranches.value(findSegmentedBranchIndex(tree->getIdentifier())).getDistanceToAngleWidget();
    }
    if (distToAngle < distToLength && distToAngle < distToWidth)
    {
        segAngleVar = true;
        angleVarType = segBranches.value(findSegmentedBranchIndex(tree->getIdentifier())).angleCheckOnLine(this->tree, mousePoint);
        branchBeingModified = tree;
        branchBeingModifiedIdentifier = branchBeingModified->getIdentifier();
        origVariationPoint = branchBeingModified->getAngleVariation(angleVarType);
        branchesBeingModifiedIdentifier.clear();
        origVariationPoints.clear();
        finalVariationPoints.clear();
        findOtherSelectedBranches(this->tree, angleVarType, "angle");
    }
    if (distToLength < distToAngle && distToLength < distToWidth)
    {
        segLengthVar = true;
        lengthVarType = segBranches.value(findSegmentedBranchIndex(tree->getIdentifier())).lengthCheckOnPoint(this->tree, mousePoint);
        branchBeingModified = tree;
        branchBeingModifiedIdentifier = branchBeingModified->getIdentifier();
        origVariationLength = branchBeingModified->getLengthVariation(lengthVarType);
        branchesBeingModifiedIdentifier.clear();
        origVariationLengths.clear();
        finalVariationLengths.clear();
        findOtherSelectedBranches(this->tree, lengthVarType, "length");

    }
    if (distToWidth < distToAngle && distToWidth < distToLength)
    {
        segThickVar = true;
        thickVarType = segBranches.value(findSegmentedBranchIndex(tree->getIdentifier())).widthCheckOnPoint(this->tree, mousePoint);
        branchBeingModified = tree;
        branchBeingModifiedIdentifier = branchBeingModified->getIdentifier();
        origVariationLength = branchBeingModified->getWidthVariation(thickVarType);
        currBranchWidthOrig = WidthChanges();
        float tempSMin = branchBeingModified->getWidthVariation("smin");
        float tempSMax = branchBeingModified->getWidthVariation("smax");
        float tempEMin = branchBeingModified->getWidthVariation("emin");
        float tempEMax = branchBeingModified->getWidthVariation("emax");
        currBranchWidthOrig.setVariation(tempSMin, tempSMax, tempEMin, tempEMax);

        branchesBeingModifiedIdentifier.clear();
        origVariationLengths.clear();
        finalVariationLengths.clear();
        origWidthChanges.clear();
        finalWidthChanges.clear();
        findOtherSelectedBranches(this->tree, thickVarType, "width");
    }

    if (!thickVar && !lengthVar && !angleVar && !segAngleVar && !segThickVar && !segLengthVar)
    {
        for (int i = 0 ; i < tree->numChildren() ; i++)
            checkVariation(tree->childAt(i), mousePoint);
    }
}

void SketchWidget::findOtherSelectedBranches(Branch* tr, QString varType, QString var)
{
    if (tr->getIsSelected())
    {
        if (tr->getIdentifier() != branchBeingModifiedIdentifier)
        {
            branchesBeingModifiedIdentifier.append(tr->getIdentifier());
            if (var.compare("width") == 0)
            {
                origVariationLengths.append(tr->getWidthVariation(varType));
                float tempSMin = tr->getWidthVariation("smin");
                float tempSMax = tr->getWidthVariation("smax");
                float tempEMin = tr->getWidthVariation("emin");
                float tempEMax = tr->getWidthVariation("emax");
                WidthChanges temp = WidthChanges();
                temp.setVariation(tempSMin, tempSMax, tempEMin, tempEMax);
                origWidthChanges.append(temp);
            }
            else if (var.compare("length") == 0)
                origVariationLengths.append(tr->getLengthVariation(varType));
            else
                origVariationPoints.append(tr->getAngleVariation(varType));
        }
    }

    for(int i = 0 ; i < tr->numChildren() ; i++)
        findOtherSelectedBranches(tr->childAt(i), varType, var);
}

void SketchWidget::writeToXMLFile(QString path)
{
    if (!treeEmpty)
        treeFileXML.createFile(tree, path);
}

void SketchWidget::undoAction()
{
    if (!changeStack.empty())
    {
        Change* temp = changeStack.pop();
        temp->setTree(tree);

        temp->reverse();
        tree = temp->getTree();

        if (temp->getType().compare("add") == 0 && ((ChangeAddBranch*) temp)->getIsTrunk())
        {
            treeEmpty = true;
            sketchEmpty();
        }

        if (temp->getType().compare("addseg") == 0 && ((ChangeAddSegmentedBranch*) temp)->getIsTrunk())
        {
            treeEmpty = true;
            sketchEmpty();
        }

        if (temp->getType().compare("remove") == 0 && ((ChangeRemoveBranch*) temp)->getIsTrunk())
        {
            treeEmpty = false;
            drawClosePoint = bp.drawClosePoint(tree, endPos - panDiff, endPos - panDiff);
            sketchNonEmpty();
        }

        if (!checkForSelectedBranch(tree))
            isBranchSelected = false;

        redoStack.push(temp);
        sketchChanged();
    }

    if (!treeEmpty)
        drawClosePoint = bp.drawClosePoint(tree, endPos, endPos);

    update();
}

void SketchWidget::redoAction()
{
    if (!redoStack.empty())
    {
        Change* temp = redoStack.pop();
        temp->setTree(tree);
        temp->apply();
        tree = temp->getTree();

        if (temp->getType().compare("add") == 0 && ((ChangeAddBranch*) temp)->getIsTrunk())
        {
            treeEmpty = false;
            drawClosePoint = bp.drawClosePoint(tree, endPos - panDiff, endPos - panDiff);
            sketchNonEmpty();
        }

        if (temp->getType().compare("addseg") == 0 && ((ChangeAddBranch*) temp)->getIsTrunk())
        {
            treeEmpty = false;
            drawClosePoint = bp.drawClosePoint(tree, endPos - panDiff, endPos - panDiff);
            sketchNonEmpty();
        }

        if (temp->getType().compare("remove") == 0 && ((ChangeRemoveBranch*) temp)->getIsTrunk())
        {
            treeEmpty = true;
            sketchEmpty();
        }

        changeStack.push(temp);
        sketchChanged();
    }

    if (!treeEmpty)
        drawClosePoint = bp.drawClosePoint(tree, endPos, endPos);

    update();
}

void SketchWidget::deleteKeyPressed()
{
    ChangeRemoveBranch* deleteBranch = new ChangeRemoveBranch(tree);
    deleteBranch->apply();
    tree = deleteBranch->getTree();

    if (deleteBranch->getIsTrunk())
    {
        treeEmpty = true;
        sketchEmpty();
    }

    changeStack.push(deleteBranch);
    redoStack.clear();

    isBranchSelected = false;

    if (!treeEmpty)
        drawClosePoint = bp.drawClosePoint(tree, endPos, endPos);

    sketchChanged();

    update();
}

bool SketchWidget::checkForSelectedBranch(Branch* tr)
{
    bool isSelected = false;
    bool isChildSelected = false;
    if (tr->getIsSelected())
        isSelected = true;
    for (int i = 0 ; i < tr->numChildren() ; i++)
        if (checkForSelectedBranch(tr->childAt(i)))
            isChildSelected = true;

    if (!isSelected && !isChildSelected)
        return false;
    else
        return true;
}

void SketchWidget::applyWidthChangeToOther(float widthChange, QString varType)
{
    for (int i = 0 ; i < branchesBeingModifiedIdentifier.size() ; i++)
        setSelectedBranchWidth(tree, branchesBeingModifiedIdentifier.at(i), widthChange, varType);
}

void SketchWidget::setSelectedBranchWidth(Branch* tr, long id, float widthChange, QString varType)
{
    if (tr->getIdentifier() == id)
    {
        if (tr->getIsSegment() && tr->getIsRootSegment())
        {
            SegmentedBranch tempSegBranch = segBranches.value(findSegmentedBranchIndex(tr->getIdentifier()));
            tempSegBranch.setWidthVariation(this->tree, widthChange, varType);
            this->tree = tempSegBranch.getTree();
            segBranches.replace(findSegmentedBranchIndex(tr->getIdentifier()), tempSegBranch);
        }
        else if (!tr->getIsSegment())
            tr->setWidthVariation(widthChange, varType);
    }
    else
    {
        for (int i = 0 ; i < tr->numChildren() ; i++)
            setSelectedBranchWidth(tr->childAt(i), id, widthChange, varType);
    }
}

void SketchWidget::setFinalWidthVariations(Branch* tr, long branchIdentifier, QString varType)
{
    if (tr->getIdentifier() == branchIdentifier)
    {
        float tempSMin = tr->getWidthVariation("smin");
        float tempSMax = tr->getWidthVariation("smax");
        float tempEMin = tr->getWidthVariation("emin");
        float tempEMax = tr->getWidthVariation("emax");
        WidthChanges tempWidthChange = WidthChanges();
        tempWidthChange.setVariation(tempSMin, tempSMax, tempEMin, tempEMax);
        finalWidthChanges.append(tempWidthChange);
        finalVariationLengths.append(tr->getWidthVariation(varType));
    }
    else
        for (int i = 0 ; i < tr->numChildren() ; i++)
            setFinalWidthVariations(tr->childAt(i), branchIdentifier, varType);
}

void SketchWidget::applyLengthChangeToOther(float lengthChange, QString varType)
{
    for (int i = 0 ; i < branchesBeingModifiedIdentifier.size() ; i++)
        setSelectedBranchLength(tree, branchesBeingModifiedIdentifier.at(i), lengthChange, varType);
}

void SketchWidget::setSelectedBranchLength(Branch* tr, long id, float lengthChange, QString varType)
{
    if (tr->getIdentifier() == id)
    {
        if (tr->getIsSegment() && tr->getIsRootSegment())
        {
            SegmentedBranch tempSegBranch = segBranches.value(findSegmentedBranchIndex(tr->getIdentifier()));
            tempSegBranch.setLengthVariation(this->tree, lengthChange, varType);
            this->tree = tempSegBranch.getTree();
            segBranches.replace(findSegmentedBranchIndex(tr->getIdentifier()), tempSegBranch);
        }
        else if (!tr->getIsSegment())
            tr->setLengthVariation(lengthChange, varType);
    }
    else
        for (int i = 0 ; i < tr->numChildren() ; i++)
            setSelectedBranchLength(tr->childAt(i), id, lengthChange, varType);
}

void SketchWidget::setFinalLengthVariations(Branch* tr, long branchIdentifier, QString varType)
{
    if (tr->getIdentifier() == branchIdentifier)
        finalVariationLengths.append(tr->getLengthVariation(varType));
    else
        for (int i = 0 ; i < tr->numChildren() ; i++)
            setFinalLengthVariations(tr->childAt(i), branchIdentifier, varType);
}

void SketchWidget::applyAngleChangeToOther(float angleChange, QString varType)
{
    for (int i = 0 ; i < branchesBeingModifiedIdentifier.size() ; i++)
        setSelectedBranchAngle(tree, branchesBeingModifiedIdentifier.at(i), angleChange, varType);
}

void SketchWidget::setSelectedBranchAngle(Branch* tr, long id, float angleChange, QString varType)
{
    if(tr->getIdentifier() == id)
    {
        QLineF tempLine = QLineF(tr->getStartPos(), tr->getLengthMaxPoint());
        tempLine.setAngle(tempLine.angle() + angleChange);
        tempLine.setLength(40);
        tr->setAngleVariation(tempLine.p2(), varType);
    }
    else
        for (int i = 0 ; i < tr->numChildren() ; i++)
            setSelectedBranchAngle(tr->childAt(i), id, angleChange, varType);
}

void SketchWidget::setFinalAngleVariations(Branch* tr, long branchIdentifier, QString varType)
{
    if (tr->getIdentifier() == branchIdentifier)
        finalVariationPoints.append(tr->getAngleVariation(varType));
    else
        for (int i = 0 ; i < tr->numChildren() ; i++)
            setFinalAngleVariations(tr->childAt(i), branchIdentifier, varType);
}

void SketchWidget::toggleMode()
{
    if (lineMode)
    {
        lineMode = false;
        pencilMode = true;
        selectMode = false;
    }
    else if (pencilMode)
    {
        lineMode = false;
        pencilMode = false;
        selectMode = true;
    }
    else
    {
        lineMode = true;
        pencilMode = false;
        selectMode = false;
    }
}

void SketchWidget::ctrlPressed()
{
    ctrlKeyDown = true;
}

void SketchWidget::ctrlReleased()
{
    ctrlKeyDown = false;
}

void SketchWidget::setBranchStatus(long iden, Branch* tr, bool setTo)
{
    if (tr->getIdentifier() == iden)
        tr->setIsSelected(setTo);
    else
        for (int i = 0 ; i < tr->numChildren() ; i++)
            setBranchStatus(iden, tr->childAt(i), setTo);
}

int SketchWidget::findSegmentedBranchIndex(long branchIdentifier)
{
    int index = -1;

    for (int i = 0 ; i < segBranches.size() ; i++)
    {
        if ((segBranches.value(i)).hasMember(branchIdentifier))
            index = i;
    }

    return index;
}

void SketchWidget::selectAll()
{
    if (ctrlKeyDown)
    {
        selectAllBranches(tree);
        isBranchSelected = true;
    }
    update();
}

void SketchWidget::deselectAll()
{
    if (!treeEmpty)
        deselectBranch(tree);
    isBranchSelected = false;
    update();
}

void SketchWidget::setLineMode()
{
    lineMode = true;
    pencilMode = false;
    selectMode = false;
}

void SketchWidget::setPencilMode()
{
    lineMode = false;
    pencilMode = true;
    selectMode = false;
}

void SketchWidget::setSelectMode()
{
    lineMode = false;
    pencilMode = false;
    selectMode = true;
}

void SketchWidget::newSketch()
{
    treeEmpty = true;
    tree = new Branch();
    changeStack.clear();
    redoStack.clear();
    isBranchSelected = false;
    segBranches.clear();
    sketchEmpty();
    sketchChanged();
    update();
}


void SketchWidget::findRootSegment(Branch* tr, long iden)
{
    if (tr->getIdentifier() == iden)
    {
        closestBranch = tr;
    }
    else
        for(int i = 0 ; i < tr->numChildren() ; i++)
            findRootSegment(tr->childAt(i), iden);
}

void SketchWidget::setInitialWidth(float initWidth)
{
    defaultThickness = initWidth;
}

QVector<QLineF> SketchWidget::applyPanDiffToBranchSegs(QVector<QLineF> branchSegs, QPointF pDiff)
{
    for (int i = 0 ; i < branchSegs.size() ; i++)
        branchSegs.replace(i, QLineF(branchSegs.at(i).p1() - pDiff, branchSegs.at(i).p2() - pDiff));

    return branchSegs;
}

QPointF* SketchWidget::drawFirstCurrentLine(QPainter *painter, QLineF currentLine, float width)
{
    // Draw main polygon
    QPointF rectanglePoints[4];
    QLineF normalLine = currentLine.normalVector();
    rectanglePoints[0] = calcVarExtremePoint(normalLine, currentLine.p1(), width);
    rectanglePoints[1] = findAdjacentPoint(currentLine.p1(), rectanglePoints[0]);
    rectanglePoints[3] = calcVarExtremePoint(normalLine, currentLine.p2(), width);
    rectanglePoints[2] = findAdjacentPoint(currentLine.p2(), rectanglePoints[3]);

    painter->drawPolygon(rectanglePoints, 4);

    QPointF* returnTopPoints = new QPointF[2];
    returnTopPoints[0] = rectanglePoints[3];
    returnTopPoints[1] = rectanglePoints[2];
    return returnTopPoints;
}

QPointF* SketchWidget::drawFollowingCurrentLine(QPainter* painter, QLineF currentLine, float width, QPointF* bottomPoints)
{
    // Draw main polygon
    QPointF rectanglePoints[4];
    QLineF normalLine = currentLine.normalVector();
    rectanglePoints[0] = calcVarExtremePoint(normalLine, currentLine.p1(), width);
    rectanglePoints[1] = findAdjacentPoint(currentLine.p1(), rectanglePoints[0]);
    rectanglePoints[3] = calcVarExtremePoint(normalLine, currentLine.p2(), width);
    rectanglePoints[2] = findAdjacentPoint(currentLine.p2(), rectanglePoints[3]);

    painter->drawPolygon(rectanglePoints, 4);

    // Draw connecting polygon
    QPointF connectingPoly[4];
    connectingPoly[0] = bottomPoints[0];
    connectingPoly[1] = bottomPoints[1];
    connectingPoly[2] = rectanglePoints[1];
    connectingPoly[3] = rectanglePoints[0];

    painter->drawPolygon(connectingPoly, 4);

    QPointF* returnTopPoints = new QPointF[2];
    returnTopPoints[0] = rectanglePoints[3];
    returnTopPoints[1] = rectanglePoints[2];
    return returnTopPoints;
}

QPointF SketchWidget::calcVarExtremePoint(QLineF norm, QPointF point, float len)
{
    norm.setLength(len);
    QPointF newPoint = norm.p2();
    QPointF tempPoint = norm.p1() - point;
    newPoint = newPoint - tempPoint;
    return newPoint;
}

QPointF SketchWidget::findAdjacentPoint(QPointF mid, QPointF point)
{
    QPointF newPoint;
    newPoint.setX(midpointCalculation(mid.x(), point.x()));
    newPoint.setY(midpointCalculation(mid.y(), point.y()));
    return newPoint;
}

float SketchWidget::midpointCalculation(float mid, float point)
{
    float newFloat = 2 * mid - point;
    return newFloat;
}

void SketchWidget::loadSketch(QString path)
{
    LoadSketch ls = LoadSketch(path);
    tree = ls.getTree();
    segBranches = ls.getCurvedBranches();
    identifierIncrementer = ls.getIdentifierIncrementer();

    for (int i = 0 ; i < segBranches.size() ; i++)
    {
        SegmentedBranch tempSegBranch = segBranches.value(i);
        tempSegBranch.setLengthBranchPercentages(tree);
        segBranches.replace(i, tempSegBranch);
    }

    treeEmpty = false;
    update();
}
