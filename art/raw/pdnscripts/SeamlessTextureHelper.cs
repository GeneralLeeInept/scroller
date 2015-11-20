void Render(Surface dst, Surface src, Rectangle rect)
{
    PdnRegion selectionRegion = EnvironmentParameters.GetSelection(src.Bounds);
    Rectangle selection = this.EnvironmentParameters.GetSelection(src.Bounds).GetBoundsInt();
    int CenterX = (int)(((selection.Right - selection.Left) / 2)+selection.Left);
    int CenterY = (int)(((selection.Bottom - selection.Top) / 2)+selection.Top);
    ColorBgra CurrentPixel;
    int srcX = 0, srcY = 0;
    for(int y = rect.Top; y < rect.Bottom; y++)
    {
        for (int x = rect.Left; x < rect.Right; x++)
        {
            if (selectionRegion.IsVisible(x, y))
            {
                srcX = x;
                srcY = y;
                if (x < CenterX)
                {
                    srcX += CenterX;
                }
                else
                {
                    srcX -= CenterX;
                }
                if (y < CenterY)
                {
                    srcY += CenterY;
                }
                else
                {
                    srcY -= CenterY;
                }
                dst[x,y] = src[srcX,srcY];
            }
        }
    }
} 